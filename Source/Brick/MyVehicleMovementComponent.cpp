// Fill out your copyright notice in the Description page of Project Settings.
#include "MyVehicleMovementComponent.h"

#include "CollisionQueryParams.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UMyVehicleMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;

	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start - UpdatedComponent->GetUpVector() * 2000.f;

	// Raycast
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, -1.f, 0, 1.f);


	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams)) {
		AlignmentNormal = Hit.ImpactNormal;
		float LocX = UpdatedComponent->GetComponentLocation().X;
		float LocY = UpdatedComponent->GetComponentLocation().Y;
		UpdatedComponent->SetWorldLocation(FVector(LocX, LocY, Hit.Location.Z + 100.f + 10.f * FMath::Cos(5.f * UGameplayStatics::GetRealTimeSeconds(GetWorld()))));
	}
	else {
		AlignmentNormal = FVector(0.f, 0.f, 1.f);
	}

	FVector CurrentUpVector = UpdatedComponent->GetUpVector();

	// Up Vector
	DrawDebugLine(GetWorld(), Start, Start + CurrentUpVector * 100.f, FColor::Yellow, false, -1.f, 0, 2.f);

	float AlignmentDiff = 1.f - FVector::DotProduct(CurrentUpVector.GetSafeNormal(), AlignmentNormal.GetSafeNormal());
	FVector TorqueAxis = FVector::CrossProduct(AlignmentNormal, CurrentUpVector).GetSafeNormal();
	UpdatedPrimitive->AddWorldRotation(FQuat::MakeFromEuler(TorqueAxis * DeltaTime * AlignmentDiff * 600.f));
}

void UMyVehicleMovementComponent::Accelerate(float value)
{
	FHitResult Hit(1.0f);
	SafeMoveUpdatedComponent(UpdatedComponent->GetForwardVector() * value, UpdatedComponent->GetComponentRotation(), true, Hit);
}

void UMyVehicleMovementComponent::Brake(float value)
{
	FHitResult Hit(1.0f);
	SafeMoveUpdatedComponent(UpdatedComponent->GetForwardVector() * -value, UpdatedComponent->GetComponentRotation(), true, Hit);
}

void UMyVehicleMovementComponent::SteerRight(float value)
{

}