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

	if (FMath::Abs(GasInput) > SMALL_NUMBER) {
		ThrottleValue += DeltaTime * GasInput;
	}
	else {
		ThrottleValue -= DeltaTime * 2.f;
	}

	ThrottleValue = FMath::Clamp(ThrottleValue, 0.f, 1.f);

	EngineRPM += DeltaTime * FMath::Lerp(-3000, 5000, ThrottleValue);
	EngineRPM = FMath::Clamp(EngineRPM, 0.f, 7000.f);

	float TargetEngineTorque = 0.f;
	if (EngineTorqueRPMCurve) {
		TargetEngineTorque = EngineTorqueRPMCurve->GetFloatValue(EngineRPM) * ThrottleValue;
	}
	EngineTorque = FMath::Lerp(-100.f, TargetEngineTorque, ThrottleValue);
	EngineAngularAcceleration = EngineTorque / EngineInertia;
	EngineAngularVelocity += EngineAngularAcceleration * DeltaTime;
	EngineAngularVelocity = FMath::Clamp(EngineAngularVelocity, 0.f, 7000.f * ((2.f * PI) / 60.f));

	WheelTorque = EngineTorque * MainGearRatio * CurrentGear * 0.5f;
	MaxWheelSpeed = EngineAngularVelocity / (MainGearRatio * CurrentGear);

	Suspension_FL->SetWheelTorque(WheelTorque);
	Suspension_FR->SetWheelTorque(WheelTorque);
	Suspension_FL->SetMaxWheelSpeed(MaxWheelSpeed);
	Suspension_FR->SetMaxWheelSpeed(MaxWheelSpeed);

	// Front/Real/All wheel drive
// 	Suspension_FL->SetGasInput(GasInput);
// 	Suspension_FR->SetGasInput(GasInput);
// 	Suspension_RL->SetGasInput(GasInput);
// 	Suspension_RR->SetGasInput(GasInput);

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, FString::Printf(TEXT("SteerAngleLeft: %.2f"), SteerAngleLeft));
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, FString::Printf(TEXT("SteerAngleRight: %.2f"), SteerAngleRight));
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Purple, FString::Printf(TEXT("GasInput: %.2f"), GasInput));
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Purple, FString::Printf(TEXT("ThrottleValue: %.2f"), ThrottleValue));
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Purple, FString::Printf(TEXT("EngineRPM: %.2f"), EngineRPM));
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Purple, FString::Printf(TEXT("TargetEngineTorque: %.2f"), TargetEngineTorque));
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Purple, FString::Printf(TEXT("EngineTorque: %.2f"), EngineTorque));
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Purple, FString::Printf(TEXT("EngineAngularAccel: %.2f"), EngineAngularAcceleration));
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Purple, FString::Printf(TEXT("EngineAngularVelocity: %.2f"), EngineAngularVelocity));
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, FString::Printf(TEXT("WheelTorque: %.2f"), WheelTorque));
	}
}

void UMyVehicleMovementComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	
}

void UMyVehicleMovementComponent::MoveForward(float AxisValue)
{
	GasInput = AxisValue;
}

void UMyVehicleMovementComponent::SteerRight(float AxisValue)
{
	SteerInput = AxisValue;
}

void UMyVehicleMovementComponent::HandBrake(bool Value)
{
}

void UMyVehicleMovementComponent::ToggleDebug()
{
	IsDebugging = !IsDebugging;

	GetOwner()->GetRootComponent()->SetVisibility(!IsDebugging);

	Suspension_FL->ToggleIsDebugging();
	Suspension_FR->ToggleIsDebugging();
	Suspension_RL->ToggleIsDebugging();
	Suspension_RR->ToggleIsDebugging();
}
