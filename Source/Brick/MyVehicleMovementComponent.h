// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "MyVehicleMovementComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BRICK_API UMyVehicleMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
public:
	virtual void	TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Accelerate(float value); // [0.f, 1.f]
	void Brake(float value); //[0.f, 1.f]
	void SteerRight(float value); //[-1.f, 1.f]

private:
	FVector TractionForce;
	FVector SteerTorque;
	FVector AlignmentNormal;
	float MaxSpeed = 2000.f;
};
