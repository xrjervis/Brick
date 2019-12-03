// Fill out your copyright notice in the Description page of Project Settings.

#include "SimpleVehicleMovementComponent.h"

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
USimpleVehicleMovementComponent::USimpleVehicleMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void USimpleVehicleMovementComponent::BeginPlay()
{
	Super::BeginPlay();

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


// Called every frame
void USimpleVehicleMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Ackermann steering
	float SteerAngleLeftTarget;
	float SteerAngleRightTarget;
	if (SteerInput > SMALL_NUMBER) {
		SteerAngleLeftTarget = SteerInput * FMath::RadiansToDegrees(FMath::Atan2(WheelBase, TurnRadius + 0.5f * RearTrack));
		SteerAngleRightTarget = SteerInput * FMath::RadiansToDegrees(FMath::Atan2(WheelBase, TurnRadius - 0.5f * RearTrack));
	}
	else if (SteerInput < -SMALL_NUMBER) {
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

void USimpleVehicleMovementComponent::Accelerate(float AxisValue)
{
	GasInput = AxisValue;

	Suspension_FL->SetGasInput(GasInput);
	Suspension_FR->SetGasInput(GasInput);
}

void USimpleVehicleMovementComponent::Brake(float AxisValue)
{
	BrakeInput = AxisValue;

	Suspension_FL->SetBrakeInput(BrakeInput);
	Suspension_FR->SetBrakeInput(BrakeInput);
	Suspension_RL->SetBrakeInput(BrakeInput);
	Suspension_RR->SetBrakeInput(BrakeInput);
}

void USimpleVehicleMovementComponent::SteerRight(float AxisValue)
{
	SteerInput = AxisValue;
}

void USimpleVehicleMovementComponent::ToggleDebug()
{
	IsDebugging = !IsDebugging;

	GetOwner()->GetRootComponent()->SetVisibility(!IsDebugging);

	Suspension_FL->ToggleIsDebugging();
	Suspension_FR->ToggleIsDebugging();
	Suspension_RL->ToggleIsDebugging();
	Suspension_RR->ToggleIsDebugging();
}