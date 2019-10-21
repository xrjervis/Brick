// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "MyPawnMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class BRICK_API UMyPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
public:
	virtual void	TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void	StopMovementImmediately() override;
	void			Jump();

private:
	float			_GetGravityZ() const;
	bool			_ShouldApplyGracity() const;
	FVector			_LimitVelocity(FVector NewVelocity) const;
	float			_GetMaxSpeed() const;
	FVector			_ConstrainVectorToPlane(FVector InVelocity) const;

private:
	bool	bPendingStopMovement = false;

	bool	bPendingJump = false;

	bool	bJumping = false;

	float	GravityScale = 1.0f;

	/** Limit on speed of projectile (0 means no limit). */
	float	MaxSpeed = 1000.f;

	bool	bConstrainToPlane = false;

	FVector Acceleration = FVector(0.f);

	FVector PlaneConstraintNormal = FVector(0.0f, 0.0f, 1.0f);
};
