// Fill out your copyright notice in the Description page of Project Settings.
#include "MyVehicleMovementComponent.h"

#include "CollisionQueryParams.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Math/NumericLimits.h"
#include "Misc/AssertionMacros.h"
#include "SuspensionComponent.h"

UMyVehicleMovementComponent::UMyVehicleMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMyVehicleMovementComponent::BeginPlay()
{
	TArray<USceneComponent*> ChildrenComponents;
	GetOwner()->GetRootComponent()->GetChildrenComponents(false, ChildrenComponents);

	for (auto Child : ChildrenComponents) {
		if (Child->GetFName() == TEXT("Suspension_FL")) {
			Suspension_FL = Cast<USuspensionComponent>(Child);
		}
		else if (Child->GetFName() == TEXT("Suspension_FR")) {
			Suspension_FR = Cast<USuspensionComponent>(Child);
		}
		else if (Child->GetFName() == TEXT("Suspension_RL")) {
			Suspension_RL = Cast<USuspensionComponent>(Child);
		}
		else if (Child->GetFName() == TEXT("Suspension_RR")) {
			Suspension_RR = Cast<USuspensionComponent>(Child);
		}
	}

	check(Suspension_FL);
	check(Suspension_FR);
	check(Suspension_RL);
	check(Suspension_RR);
}

void UMyVehicleMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (FMath::Abs(InputAxisValueY) > SMALL_NUMBER) {
		SteeringAngle += InputAxisValueY * 45.f * DeltaTime;
		SteeringAngle = FMath::Clamp(SteeringAngle, MinSteeringAngle, MaxSteeringAngle);
	}
	else {
		SteeringAngle *= 0.9f;
	}

	//X-Roll, Y-Pitch, Z-Yaw
	Suspension_FL->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0.f, 0.f, SteeringAngle)));
	Suspension_FR->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0.f, 0.f, SteeringAngle)));

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, FString::Printf(TEXT("SteeringAngle: %.2f"), SteeringAngle));
	}
}

void UMyVehicleMovementComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	
}

void UMyVehicleMovementComponent::Accelerate(float value)
{
}

void UMyVehicleMovementComponent::Brake(float value)
{
}

void UMyVehicleMovementComponent::MoveForward(float AxisValue)
{
	InputAxisValueX = AxisValue;
}

void UMyVehicleMovementComponent::SteerRight(float AxisValue)
{
	InputAxisValueY = AxisValue;
}