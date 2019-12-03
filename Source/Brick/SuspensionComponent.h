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
	void SetBrakeInput(float InputValue);

	void SetIsDebugging(bool enabled);
	bool GetIsDebugging() const;
	void ToggleIsDebugging();

	float GetWheelInertia() const;

public:
	UPROPERTY(EditAnywhere, Category = "Wheel")
	float Radius = 35.f; //cm
	UPROPERTY(EditAnywhere, Category = "Wheel")
	float Mass = 15.f; //kg

	UPROPERTY(EditAnywhere, Category = "Suspension")
	float RestLength = 50.f;
	UPROPERTY(EditAnywhere, Category = "Suspension")
	float TravelLength = 20.f;
	UPROPERTY(EditAnywhere, Category = "Suspension")
	float SpringStiffness = 50000.f;
	UPROPERTY(EditAnywhere, Category = "Suspension")
	float DamperStiffness = 1500.f;
	UPROPERTY(EditAnywhere, Category = "Suspension")
	float CorneringStiffness = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Suspension")
	float LongStiffness = 1.f;
	UPROPERTY(EditAnywhere, Category = "Suspension")
	float MinForce = -2000.f;
	UPROPERTY(EditAnywhere, Category = "Suspension")
	float MaxForce = 6000.f;
	UPROPERTY(EditAnywhere, Category = "Suspension")
	UCurveFloat* TireForceSlipCurve = nullptr;

	bool IsDebugging = false;

	UPrimitiveComponent* Body = nullptr;
	UPrimitiveComponent* WheelMesh = nullptr;
	
	float GasInput = 0.f;
	float BrakeInput = 0.f;

	float SpringLength = RestLength;
	FVector WorldWheelVelocity;
	FVector LocalWheelVelocity;

	float WheelAngularVelocity; // rad/s
};
