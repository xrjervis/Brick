// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "MyVehicleMovementComponent.generated.h"

UCLASS(Blueprintable)
class BRICK_API UMyVehicleMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Accelerate(float value); // [0.f, 1.f]
	void Brake(float value); //[0.f, 1.f]
	void SteerRight(float value); //[-1.f, 1.f]

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vehicle")
	float EnginePower = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Vehicle")
	float MaxEnginePower = 4000.f;

private:
	FVector TractionForce;
	FVector SteerTorque;

	FVector AlignmentNormal;

	float AccelerateValue = 0.f;
	float BrakeValue = 0.f;
	float SteerValue = 0.f;
};