// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SuspensionComponent.generated.h"

class UStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRICK_API USuspensionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USuspensionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetGasInput(float InputValue);

	void SetHandBraking(bool value);

public:
	UPROPERTY(EditAnywhere, Category = "Wheel")
	float Radius = 35.f;
	UPROPERTY(EditAnywhere, Category = "Wheel")
	float Mass = 15.f;

	UPROPERTY(EditAnywhere, Category = "Suspension")
	float RestLength = 50.f;
	UPROPERTY(EditAnywhere, Category = "Suspension")
	float TravelLength = 20.f;
	UPROPERTY(EditAnywhere, Category = "Suspension")
	float SpringStiffness = 50000.f;
	UPROPERTY(EditAnywhere, Category = "Suspension")
	float DamperStiffness = 1500.f;
	UPROPERTY(EditAnywhere, Category = "Suspension")
	float MinForce = -2000.f;
	UPROPERTY(EditAnywhere, Category = "Suspension")
	float MaxForce = 6000.f;

	UPrimitiveComponent* Body = nullptr;
	UPrimitiveComponent* WheelMesh = nullptr;
	
	bool IsHandBraking = false;

	float GasInput = 0.f;

	float SpringLength = RestLength;
	FVector WorldWheelVelocity;
	FVector LocalWheelVelocity;
	float AngularVelocity; // rad/s
};
