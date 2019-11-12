// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "MyVehicleMovementComponent.generated.h"

class USuspensionComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICK_API UMyVehicleMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UMyVehicleMovementComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	void Accelerate(float value); // [0.f, 1.f]
	void Brake(float value); //[0.f, 1.f]

	UFUNCTION(BlueprintCallable, Category = "VehicleMovement")
	void MoveForward(float AxisValue); //[-1.f, 1.f]
	UFUNCTION(BlueprintCallable, Category = "VehicleMovement")
	void SteerRight(float AxisValue); //[-1.f, 1.f]

public:
	UPROPERTY(EditAnywhere, Category = "VehicleMovement")
	float MinSteeringAngle = -45.f;
	UPROPERTY(EditAnywhere, Category = "VehicleMovement")
	float MaxSteeringAngle = 45.f;

private:
	float InputAxisValueX = 0.f;
	float InputAxisValueY = 0.f;
	float SteeringAngle = 0.f;

	USuspensionComponent* Suspension_FL = nullptr;
	USuspensionComponent* Suspension_FR = nullptr;
	USuspensionComponent* Suspension_RL = nullptr;
	USuspensionComponent* Suspension_RR = nullptr;
};