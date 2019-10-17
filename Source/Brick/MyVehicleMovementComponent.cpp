// Fill out your copyright notice in the Description page of Project Settings.
#include "MyVehicleMovementComponent.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/Engine.h"

void UMyVehicleMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (UpdatedComponent->IsSimulatingPhysics()) {
		FVector InputForce = ConsumeInputVector().GetClampedToMaxSize(1.0f) * 300.f;
		UpdatedPrimitive->AddForce(InputForce);
	}
}
