// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SimpleVehicleMovementComponent.generated.h"

class USuspensionComponent;
class UStaticMeshComponent;
class USceneComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRICK_API USimpleVehicleMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USimpleVehicleMovementComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "VehicleMovement")
	void Accelerate(float AxisValue); //[0.f, 1.f]
	UFUNCTION(BlueprintCallable, Category = "VehicleMovement")
	void Brake(float AxisValue); //[0.f, 1.f]
	UFUNCTION(BlueprintCallable, Category = "VehicleMovement")
	void SteerRight(float AxisValue); //[-1.f, 1.f]
	UFUNCTION(BlueprintCallable, Category = "VehicleMovement")
	void ToggleDebug();

public:
	UPROPERTY(EditAnywhere, Category = "VehicleMovement")
	float TurnRadius = 600.f;

private:
	bool IsDebugging = false;

	USuspensionComponent* Suspension_FL = nullptr;
	USuspensionComponent* Suspension_FR = nullptr;
	USuspensionComponent* Suspension_RL = nullptr;
	USuspensionComponent* Suspension_RR = nullptr;

	float SteerInput = 0.f;
	float SteerAngleLeft = 0.f;
	float SteerAngleRight = 0.f;
	float WheelBase;
	float RearTrack;

	float GasInput = 0.f; //[0.f, 1.f]
	float BrakeInput = 0.f; //[0.f, 1.f]
};
