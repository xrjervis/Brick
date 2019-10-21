// Fill out your copyright notice in the Description page of Project Settings.
#include "MyVehicleMovementComponent.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/Engine.h"

void UMyVehicleMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (UpdatedComponent->IsSimulatingPhysics()) {
		UpdatedPrimitive->AddForce(TractionForce);
		UpdatedPrimitive->SetPhysicsLinearVelocity(UpdatedPrimitive->GetPhysicsLinearVelocity().GetClampedToMaxSize(2000.f));
		TractionForce = FVector::ZeroVector;
	}
}

void UMyVehicleMovementComponent::Accelerate(float value)
{
	TractionForce += FVector(UpdatedComponent->GetForwardVector() * 2000.f);
}

void UMyVehicleMovementComponent::Brake(float value)
{
	TractionForce += FVector(-UpdatedComponent->GetForwardVector() * 2000.f);
}

void UMyVehicleMovementComponent::SteerRight(float value)
{

}
