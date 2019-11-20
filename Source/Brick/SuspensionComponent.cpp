// Fill out your copyright notice in the Description page of Project Settings.

#include "CollisionQueryParams.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Math/NumericLimits.h"
#include "Misc/AssertionMacros.h"
#include "SuspensionComponent.h"

// Sets default values for this component's properties
USuspensionComponent::USuspensionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void USuspensionComponent::BeginPlay()
{
	Super::BeginPlay();

	Body = Cast<UPrimitiveComponent>(GetAttachmentRoot());
	checkf(Body, TEXT("Must be attached to a root static mesh component"));

	WheelMesh = Cast<UPrimitiveComponent>(GetChildComponent(0));
	checkf(WheelMesh, TEXT("Must attach a wheel static mesh"));
}


// Called every frame
void USuspensionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Prepare for Raycast
	float RaycastLength = RestLength + TravelLength + Radius;
	FVector Start = GetComponentLocation();
	FVector End = Start - RaycastLength * GetUpVector();
	FCollisionQueryParams TraceParams(TEXT("InteractTrace"), true, nullptr);
	TraceParams.bReturnPhysicalMaterial = true;
	FHitResult HitResult;

	float LocalWheelLongTractionValue;
	float LocalWheelLateralFrictionValue;

	bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams);
	if (IsHit) {
		float LastLength = SpringLength;
		SpringLength = HitResult.Distance - Radius;
		SpringLength = FMath::Clamp(SpringLength, RestLength - TravelLength, RestLength + TravelLength);
		float SpringForce = SpringStiffness * (RestLength - SpringLength);
		float SpringVelocity = (LastLength - SpringLength) / DeltaTime;
		float DamperForce = DamperStiffness * SpringVelocity;

		WorldWheelVelocity = Body->GetPhysicsLinearVelocityAtPoint(HitResult.Location);
		LocalWheelVelocity = GetComponentTransform().InverseTransformVectorNoScale(WorldWheelVelocity);
		AngularVelocity = LocalWheelVelocity.X / Radius; // rad/s

		// Hack - If the speed is too low, snap it to zero
		// Otherwise the car will wobbling
		if (FMath::Abs(LocalWheelVelocity.Y) < 20.f) {
			LocalWheelVelocity.Y = 0.f;
		}
		LocalWheelLateralFrictionValue = FMath::Clamp(-SpringForce * LocalWheelVelocity.Y, -SpringForce, SpringForce);
		LocalWheelLongTractionValue = GasInput * 0.5f * SpringForce;

		FVector TireForce = LocalWheelLongTractionValue * GetForwardVector() + LocalWheelLateralFrictionValue * GetRightVector();
		TireForce = TireForce.GetClampedToMaxSize(SpringForce);

		FVector SuspensionForce = (SpringForce + DamperForce) * GetUpVector();
		SuspensionForce.GetClampedToSize(MinForce, MaxForce);

		Body->AddForceAtLocation(SuspensionForce, Start);
		Body->AddForceAtLocation(TireForce, HitResult.Location);

		DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector(2.f, 2.f, 2.f), FColor::Green, false, 5.f, ECC_WorldStatic, 1.f);
		DrawDebugLine(GetWorld(), Start, Start + WorldWheelVelocity, FColor::Yellow, false, -1.f, 0, 2);
		FString DebugMsg = FString::Printf(TEXT("Force: %.2f | LateralFrictionValue: %.2f"), SpringForce, LocalWheelLateralFrictionValue);
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, DebugMsg);
	}
	else {
		SpringLength = RestLength + TravelLength;
		LocalWheelLongTractionValue = 0.f;
		LocalWheelLateralFrictionValue = 0.f;
	}
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, -1.f, 0, 2);

	if (WheelMesh) {
		float WheelRotation = FMath::RadiansToDegrees(AngularVelocity * DeltaTime);
		// Flip rotation for left side wheels
		if (FVector::DotProduct(WheelMesh->GetRightVector(), GetRightVector()) > 0.f) {
			WheelRotation = -WheelRotation;
		}
		WheelMesh->AddLocalRotation(FQuat::MakeFromEuler(FVector(0.f, WheelRotation, 0.f)));
		WheelMesh->SetRelativeLocation(FVector(0.f, 0.f, -SpringLength));
	}
}

void USuspensionComponent::SetGasInput(float InputValue)
{
	GasInput = InputValue;
}

void USuspensionComponent::SetHandBraking(bool value)
{
	IsHandBraking = value;
}

