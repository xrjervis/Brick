// Fill out your copyright notice in the Description page of Project Settings.

#include "CollisionQueryParams.h"
#include "Components/PrimitiveComponent.h"
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
	check(Body != nullptr);
}


// Called every frame
void USuspensionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float RaycastLength = RestLength + TravelLength + Radius;
	FVector Start = GetComponentLocation();
	FVector End = Start - RaycastLength * GetUpVector();
	FCollisionQueryParams TraceParams(TEXT("InteractTrace"), true, nullptr);
	TraceParams.bReturnPhysicalMaterial = true;
	FHitResult HitResult;

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, -1.f, 0, 2);
	bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams);
	if (IsHit) {
		DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector(2.f, 2.f, 2.f), FColor::Green, false, -1.f, ECC_WorldStatic, 1.f);
		float LastLength = SpringLength;
		SpringLength = HitResult.Distance - Radius;
		SpringLength = FMath::Clamp(SpringLength, RestLength - TravelLength, RestLength + TravelLength);
		float SpringForce = SpringStiffness * (RestLength - SpringLength);
		float SpringVelocity = (LastLength - SpringLength) / DeltaTime;
		float DamperForce = DamperStiffness * SpringVelocity;

		FVector SuspensionForce = (SpringForce + DamperForce) * GetUpVector();
		SuspensionForce.GetClampedToSize(MinForce, MaxForce);
		Body->AddForceAtLocation(SuspensionForce, Start);

		FString DebugMsg = FString::Printf(TEXT("Force: %.2f"), SpringForce);
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, DebugMsg);
	}
}

