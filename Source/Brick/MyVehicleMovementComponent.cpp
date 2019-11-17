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

	FVector FrontWheelMidPoint = (Suspension_FL->GetComponentLocation() + Suspension_FR->GetComponentLocation()) * 0.5f;
	FVector RearWheelMidPoint = (Suspension_RL->GetComponentLocation() + Suspension_RR->GetComponentLocation()) * 0.5f;

	WheelBase = FVector::Distance(FrontWheelMidPoint, RearWheelMidPoint);
	RearTrack = FVector::Distance(Suspension_RL->GetComponentLocation(), Suspension_RR->GetComponentLocation());
}

void UMyVehicleMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	float SteerAngleLeftTarget;
	float SteerAngleRightTarget;
	if (SteerInput > SMALL_NUMBER) {
		SteerAngleLeftTarget = SteerInput * FMath::RadiansToDegrees(FMath::Atan2(WheelBase, TurnRadius + 0.5f * RearTrack));
		SteerAngleRightTarget = SteerInput * FMath::RadiansToDegrees(FMath::Atan2(WheelBase, TurnRadius - 0.5f * RearTrack));
	}
	else if (SteerInput < -SMALL_NUMBER){
		SteerAngleLeftTarget = SteerInput * FMath::RadiansToDegrees(FMath::Atan2(WheelBase, TurnRadius - 0.5f * RearTrack));
		SteerAngleRightTarget = SteerInput * FMath::RadiansToDegrees(FMath::Atan2(WheelBase, TurnRadius + 0.5f * RearTrack));
	}
	else {
		SteerAngleLeftTarget = 0.f;
		SteerAngleRightTarget = 0.f;
	}
	SteerAngleLeft = FMath::FInterpConstantTo(SteerAngleLeft, SteerAngleLeftTarget, DeltaTime, 30.f);
	SteerAngleRight = FMath::FInterpConstantTo(SteerAngleRight, SteerAngleRightTarget, DeltaTime, 30.f);

	//X-Roll, Y-Pitch, Z-Yaw
	Suspension_FL->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0.f, 0.f, SteerAngleLeft)));
	Suspension_FR->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0.f, 0.f, SteerAngleRight)));

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, FString::Printf(TEXT("SteerAngleLeft: %.2f"), SteerAngleLeft));
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, FString::Printf(TEXT("SteerAngleRight: %.2f"), SteerAngleRight));
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
	SteerInput = AxisValue;
}