// Fill out your copyright notice in the Description page of Project Settings.
#include "MyVehicleMovementComponent.h"

#include "CollisionQueryParams.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Math/NumericLimits.h"

void UMyVehicleMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	FVector Force_long(0.f);

	// Get old velocity from last frame
	FVector OldVelocity = UpdatedComponent->ComponentVelocity;
	float OldSpeed = OldVelocity.Size();

	// Fluid Mechanics: C_drag = 0.5f * C_d * Area * rho
	// C_d depends on the shape of the car and determined via wind tunnel test. Approximate value for a Corvette: 0.3
	// Air density (rho) is 1.29 kg/m3
	// frontal area is approx 2.2 m2
	constexpr float C_drag = 0.5f * 2.f * 5.f * 1.29f;
	FVector Force_drag = -C_drag * OldVelocity * OldSpeed;

	// Rolling resistance, approx. 30 times C_drag
	constexpr float C_rr = 30.f * C_drag;
	FVector Force_rr = -C_rr * OldVelocity;

	// Calculate engine power and traction force
	if (AccelerateValue > 0.f) {
		EnginePower += AccelerateValue * DeltaTime * 1500.f;
	}
	else {
		EnginePower -= DeltaTime * 4000.f;
	}
	EnginePower = FMath::Clamp(EnginePower, 0.f, MaxEnginePower);
	FVector Force_traction = UpdatedComponent->GetForwardVector() * EnginePower;

	FString DebugMsg = FString::Printf(TEXT("Engine power: %f"), EnginePower);
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, DebugMsg);
	DebugMsg = FString::Printf(TEXT("Speed: %f"), OldSpeed);
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, DebugMsg);
	
	// Calculate braking force
	FVector Force_braking(0.f);
	float C_braking = 8000.f;

	if (BrakeValue > 0.f) {
		// Do not apply braking force if speed is reduced to zero
		if (OldSpeed > 0.f) {
			Force_braking = -UpdatedComponent->GetForwardVector() * C_braking;
			Force_long += Force_braking;
		}
	}
	else {
		Force_long += Force_traction;
	}
	Force_long += Force_drag + Force_rr;

	float Mass = 1000.f; //kg

	FVector Acceleration = Force_long / Mass;

	FVector NewVelocity = OldVelocity + Acceleration * DeltaTime;

	// Prevent velocity increasing backwards
	if (BrakeValue > 0.f && (FVector::DotProduct(OldVelocity, NewVelocity) < 0.f)) {
		NewVelocity = FVector::ZeroVector;
	}

	// Steering
	// Calculate angular velocity
	const float FrontRearWheelDist = 3.2f; // meters
	float SteerDegrees = 30.f * SteerValue;
	float SteerRadius = FrontRearWheelDist / FMath::Sin(FMath::DegreesToRadians(SteerDegrees));
	float AngularSpeed = FMath::RadiansToDegrees(OldSpeed / SteerRadius);

	// Rotates vehicle's body
	UpdatedPrimitive->AddWorldRotation(FQuat::MakeFromEuler(FVector::UpVector * AngularSpeed * DeltaTime));

	// Eliminate lateral velocity
	FVector LateralVelocity = NewVelocity.ProjectOnToNormal(UpdatedPrimitive->GetRightVector());
	NewVelocity -= LateralVelocity;

	FVector DeltaMove = NewVelocity * DeltaTime * 100.f; //times 100 m -> cm

	FHitResult Hit(1.0f);
	SafeMoveUpdatedComponent(DeltaMove, UpdatedComponent->GetComponentRotation(), true, Hit);

	if (Hit.IsValidBlockingHit()) {
		NewVelocity = NewVelocity - 1.2f * NewVelocity.ProjectOnToNormal(Hit.Normal);
		DeltaMove = DeltaMove - DeltaMove.ProjectOnToNormal(Hit.Normal);

		// Handles the calculations and physics involved with sliding smoothly along collision surfaces like walls and ramps
		// when a move results in a collision, rather than simply stopping in place and sticking to the wall or ramp
		SlideAlongSurface(DeltaMove, 1.f - Hit.Time, Hit.Normal, Hit);
	}

	Velocity = NewVelocity;
	UpdateComponentVelocity();
	// -----------------------------------------------------------------------------

// 	FHitResult Hit;
// 	FCollisionQueryParams CollisionParams;
// 
// 	// Raycast
// 	FVector Start = UpdatedComponent->GetComponentLocation();
// 	FVector End = Start - UpdatedComponent->GetUpVector() * 2000.f;
// 	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, -1.f, 0, 1.f);
// 
// 	FVector CurrentUpVector = UpdatedComponent->GetUpVector();
// 
// 	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams)) {
// 		AlignmentNormal = Hit.ImpactNormal;
// 		float LocX = UpdatedComponent->GetComponentLocation().X;
// 		float LocY = UpdatedComponent->GetComponentLocation().Y;
// 		UpdatedComponent->SetWorldLocation(Hit.Location + CurrentUpVector * (100.f + 10.f * FMath::Cos(5.f * UGameplayStatics::GetRealTimeSeconds(GetWorld()))));
// 	}
// 	else {
// 		AlignmentNormal = FVector(0.f, 0.f, 1.f);
// 	}
// 
// 	// Up Vector
// 	DrawDebugLine(GetWorld(), Start, Start + CurrentUpVector * 100.f, FColor::Yellow, false, -1.f, 0, 2.f);
// 
// 	float AlignmentDiff = 1.f - FVector::DotProduct(CurrentUpVector.GetSafeNormal(), AlignmentNormal.GetSafeNormal());
// 	FVector TorqueAxis = FVector::CrossProduct(AlignmentNormal, CurrentUpVector).GetSafeNormal();
// 	UpdatedPrimitive->AddWorldRotation(FQuat::MakeFromEuler(TorqueAxis * DeltaTime * AlignmentDiff * 1000.f));
}

void UMyVehicleMovementComponent::Accelerate(float value)
{
	AccelerateValue = value;
}

void UMyVehicleMovementComponent::Brake(float value)
{
	BrakeValue = value;
}

void UMyVehicleMovementComponent::SteerRight(float value)
{
	SteerValue = value;
}