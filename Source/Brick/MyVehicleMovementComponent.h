// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "MyVehicleMovementComponent.generated.h"

class USuspensionComponent;
class UStaticMeshComponent;
class USceneComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICK_API UMyVehicleMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UMyVehicleMovementComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	UFUNCTION(BlueprintCallable, Category = "VehicleMovement")
	void MoveForward(float AxisValue); //[-1.f, 1.f]
	UFUNCTION(BlueprintCallable, Category = "VehicleMovement")
	void SteerRight(float AxisValue); //[-1.f, 1.f]
	UFUNCTION(BlueprintCallable, Category = "VehicleMovement")
	void HandBrake(bool Value);
	UFUNCTION(BlueprintCallable, Category = "VehicleMovement")
	void ToggleDebug();


public:
	UPROPERTY(EditAnywhere, Category = "VehicleMovement")
	float TurnRadius = 600.f;

	UPROPERTY(EditAnywhere, Category = "VehicleMovement")
	UCurveFloat* EngineTorqueRPMCurve = nullptr;

private:
	bool IsDebugging = false;

	USuspensionComponent* Suspension_FL = nullptr;
	USuspensionComponent* Suspension_FR = nullptr;
	USuspensionComponent* Suspension_RL = nullptr;
	USuspensionComponent* Suspension_RR = nullptr;

	float WheelBase;
	float RearTrack;

	float SteerInput = 0.f;
	float SteerAngleLeft = 0.f;
	float SteerAngleRight = 0.f;

	float GasInput = 0.f;
	int CurrentGear = 1;
	float MainGearRatio = 3.82f;
	float EngineRPM = 0.f;
	float EngineTorque = 0.f;
	float EngineInertia = 0.3f;
	float EngineAngularAcceleration = 0.f;
	float EngineAngularVelocity = 0.f;
	float WheelTorque = 0.f;
	float MaxWheelSpeed = 0.f;
};