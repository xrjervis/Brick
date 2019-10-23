// Fill out your copyright notice in the Description page of Project Settings.
#include "MyVehicleMovementComponent.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/Engine.h"

void UMyVehicleMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	static float cosValue = 0.f;
	cosValue += DeltaTime * 5.f;
	UpdatedComponent->SetWorldLocation(FVector(0.f, 0.f, 102.f + FMath::Cos(cosValue)));
}

void UMyVehicleMovementComponent::Accelerate(float value)
{
	TractionForce += FVector(UpdatedComponent->GetForwardVector() * 2000.f * value);
}

void UMyVehicleMovementComponent::Brake(float value)
{
	TractionForce += FVector(-UpdatedComponent->GetForwardVector() * 2000.f * value);
}

void UMyVehicleMovementComponent::SteerRight(float value)
{
	FVector CurrentVelocity = UpdatedPrimitive->GetPhysicsLinearVelocity();
	CurrentVelocity.RotateAngleAxis(45.f * value, UpdatedPrimitive->GetUpVector());
	UpdatedPrimitive->SetPhysicsLinearVelocity(CurrentVelocity);
}