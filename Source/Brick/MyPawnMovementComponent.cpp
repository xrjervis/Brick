// Fill out your copyright notice in the Description page of Project Settings.
#include "MyPawnMovementComponent.h"
#include "Engine/Engine.h"

void UMyPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (bPendingStopMovement) {
		Velocity = FVector::ZeroVector;
		Acceleration = FVector::ZeroVector;
		bPendingStopMovement = false;
		return;
	}

	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime) || (bIsActive == false) || UpdatedComponent->IsSimulatingPhysics()) {
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bPendingJump) {
		bPendingJump = false;
		Velocity.Z += 200.f;
	}
	if (Velocity.Size() < 1.f) {
		Velocity = FVector::ZeroVector;
	}

	FVector GravityAccel = FVector(0.f, 0.f, _GetGravityZ());
	FVector DragForce = 0.5f * 0.001f * (Velocity * Velocity.GetAbs());
	FVector DragAccel = DragForce / 1.0f; // Assume mass is 1
	FVector InputForce = ConsumeInputVector().GetClampedToMaxSize(1.0f) * 300.f;
	FVector InputAccel = InputForce / 1.0f;

	FVector NewAcceleration = InputAccel + GravityAccel - DragAccel;
	FVector NewVelocity = Velocity + (Acceleration + NewAcceleration) * (DeltaTime * 0.5f);
	FVector MoveDelta = NewVelocity * DeltaTime + NewAcceleration * DeltaTime * DeltaTime * 0.5f;

	FString DebugMsg = FString::Printf(TEXT("Speed: %.2f"), Velocity.Size());
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, DebugMsg);
	DebugMsg = FString::Printf(TEXT("Jumping: %s"), bJumping ? TEXT("true") : TEXT("false"));
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, DebugMsg);
	DebugMsg = FString::Printf(TEXT("DragAccel: (%.2f, %.2f, %.2f)"), DragAccel.X, DragAccel.Y, DragAccel.Z);
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, DebugMsg);

	// Get and then clear the movement vector that we set in MyPawn::Tick
	//FVector DesiredMovementThisFrame = MoveDelta + ConsumeInputVector().GetClampedToMaxSize(1.0f) * DeltaTime * 150.f;
	FVector DesiredMovementThisFrame = MoveDelta;
	if (!DesiredMovementThisFrame.IsNearlyZero()) {
		// Uses engine physics to move our Pawn Movement Component while respecting solid barriers
		FHitResult Hit(1.0f);
		SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);

		if (Hit.IsValidBlockingHit()) {
			// Collision reduces velocity (hard, no bounce)
			NewVelocity = NewVelocity - NewVelocity.ProjectOnToNormal(Hit.Normal);
			DesiredMovementThisFrame = DesiredMovementThisFrame - DesiredMovementThisFrame.ProjectOnToNormal(Hit.Normal);

			// Check if hit ground
			if (bJumping && (FVector::DotProduct(Hit.Normal, FVector::UpVector) > -0.4f)) {
				bJumping = false;
			}
			// Apply friction
			FVector mg = 1.f * GravityAccel;
			float FrictionMagnitude = 0.3f * mg.ProjectOnToNormal(Hit.Normal).Size(); // Coefficient depends
			FVector Friction = -NewVelocity.GetSafeNormal() * FrictionMagnitude;
			NewAcceleration += Friction;


			DebugMsg = FString::Printf(TEXT("Friction: %.2f"), FrictionMagnitude);
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, DebugMsg);
			DebugMsg = FString::Printf(TEXT("Hit.Time: %f"), Hit.Time);
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, DebugMsg);


			// Handles the calculations and physics involved with sliding smoothly along collision surfaces like walls and ramps
			// when a move results in a collision, rather than simply stopping in place and sticking to the wall or ramp
			SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
		}
	}

	Velocity = NewVelocity;
	Acceleration = NewAcceleration;

	UpdateComponentVelocity();
}

void UMyPawnMovementComponent::StopMovementImmediately() 
{
	bPendingStopMovement = true;
}

void UMyPawnMovementComponent::Jump()
{
	if (bJumping || bPendingJump) {
		return;
	}
	bPendingJump = true;
	bJumping = true;
}

float UMyPawnMovementComponent::_GetGravityZ() const
{
	// TODO: apply buoyancy if in water
	return _ShouldApplyGracity() ? (Super::GetGravityZ() * GravityScale) : 0.0f;
}

bool UMyPawnMovementComponent::_ShouldApplyGracity() const
{
	return (GravityScale != 0.f);
}

FVector UMyPawnMovementComponent::_LimitVelocity(FVector NewVelocity) const
{
	const float CurrentMaxSpeed = _GetMaxSpeed();
	if (CurrentMaxSpeed > 0.f) {
		NewVelocity = NewVelocity.GetClampedToMaxSize(CurrentMaxSpeed);
	}
	return _ConstrainVectorToPlane(NewVelocity);
}

float UMyPawnMovementComponent::_GetMaxSpeed() const
{
	return MaxSpeed;
}

FVector UMyPawnMovementComponent::_ConstrainVectorToPlane(FVector InVelocity) const
{
	if (bConstrainToPlane) {
		InVelocity = FVector::VectorPlaneProject(InVelocity, PlaneConstraintNormal);
	}
	return InVelocity;
}
