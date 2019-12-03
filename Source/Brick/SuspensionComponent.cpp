// Fill out your copyright notice in the Description page of Project Settings.

#include "CollisionQueryParams.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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

	float SpringForce;
	FVector TireForce;
	FVector2D LocalTireForce = FVector2D::ZeroVector;

	WorldWheelVelocity = Body->GetPhysicsLinearVelocityAtPoint(WheelMesh->GetComponentLocation() - Radius * GetUpVector());
	LocalWheelVelocity = GetComponentTransform().InverseTransformVectorNoScale(WorldWheelVelocity * 0.01f); // m/s

	WheelAngularVelocity = LocalWheelVelocity.X / (Radius * 0.01f); // omega

	bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams);
	if (IsHit) {
		float LastLength = SpringLength;
		SpringLength = HitResult.Distance - Radius;
		SpringLength = FMath::Clamp(SpringLength, RestLength - TravelLength, RestLength + TravelLength);
		SpringForce = SpringStiffness * (RestLength - SpringLength);
		float SpringVelocity = (LastLength - SpringLength) / DeltaTime;
		float DamperForce = DamperStiffness * SpringVelocity;

		float LocalLateralSlipNorm = FMath::Clamp(LocalWheelVelocity.Y * -CorneringStiffness, -1.f, 1.f);
		float LocalLongSlipNorm = FMath::Clamp(LocalWheelVelocity.X * -LongStiffness, -1.f, 1.f);
		if (GasInput) {
			LocalLongSlipNorm = GasInput;
		}
		if (BrakeInput) {
			LocalLongSlipNorm = FMath::Clamp(LocalWheelVelocity.X * -(UKismetMathLibrary::MapRangeClamped(BrakeInput, 0.f, 1.f, LongStiffness, 3.f * LongStiffness)), -1.f, 1.f);
			LocalLateralSlipNorm = FMath::Clamp(LocalWheelVelocity.Y * -(UKismetMathLibrary::MapRangeClamped(BrakeInput, 0.f, 1.f, CorneringStiffness, 0.1f * CorneringStiffness)), -1.f, 1.f);
		}
		

		auto CombinedVector2D = FVector2D(LocalLongSlipNorm, LocalLateralSlipNorm);
		float SlipSize = CombinedVector2D.Size();
		CombinedVector2D.Normalize();

		auto LocalTireForceNormalized = TireForceSlipCurve->GetFloatValue(SlipSize) * CombinedVector2D;
		LocalTireForce = FMath::Max(SpringForce * 0.01f, 0.f) * LocalTireForceNormalized;

		TireForce = LocalTireForce.X * GetForwardVector() + LocalTireForce.Y * GetRightVector();

		FVector SuspensionForce = (SpringForce + DamperForce) * GetUpVector();
		SuspensionForce.GetClampedToSize(MinForce, MaxForce);

		Body->AddForceAtLocation(SuspensionForce, Start);
		Body->AddForceAtLocation(TireForce * 100.f, HitResult.Location);

		DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector(2.f, 2.f, 2.f), FColor::Green, false, 5.f, ECC_WorldStatic, 1.f);
	}
	else {
		SpringLength = RestLength + TravelLength;
		SpringForce = SpringStiffness * (-TravelLength);
	}

	if (WheelMesh) {
		float WheelRotation = FMath::RadiansToDegrees(WheelAngularVelocity * DeltaTime);
		// Flip rotation for left side wheels
		if (FVector::DotProduct(WheelMesh->GetRightVector(), GetRightVector()) > 0.f) {
			WheelRotation = -WheelRotation;
		}
		WheelMesh->AddLocalRotation(FQuat::MakeFromEuler(FVector(0.f, WheelRotation, 0.f)));
		WheelMesh->SetRelativeLocation(FVector(0.f, 0.f, -SpringLength));
	}

	if (IsDebugging) {
		DrawDebugLine(GetWorld(), Start, Start + GetForwardVector() * LocalTireForce.X * 0.01f, FColor::Red, false, -1.f, ECC_WorldStatic, 5.f);
		DrawDebugLine(GetWorld(), Start, Start + GetRightVector() * LocalTireForce.Y * 0.01f, FColor::Green, false, -1.f, ECC_WorldStatic, 5.f);
		DrawDebugLine(GetWorld(), Start, Start + GetUpVector() * SpringForce * 0.0001f, FColor::Blue, false, -1.f, ECC_WorldStatic, 5.f);
	}
}

void USuspensionComponent::SetGasInput(float InputValue)
{
	GasInput = InputValue;
}

void USuspensionComponent::SetBrakeInput(float InputValue)
{
	BrakeInput = InputValue;
}

void USuspensionComponent::SetIsDebugging(bool enabled)
{
	IsDebugging = enabled;
}

bool USuspensionComponent::GetIsDebugging() const
{
	return IsDebugging;
}

void USuspensionComponent::ToggleIsDebugging()
{
	IsDebugging = !IsDebugging;
}

float USuspensionComponent::GetWheelInertia() const
{
	return (Radius * 0.01f) * (Radius * 0.01f) * Mass * 0.5f;
}
