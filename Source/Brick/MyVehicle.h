// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyVehicle.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UCameraComponent;
class UMyVehicleMovementComponent;
class USpringArmComponent;
class USuspensionComponent;

UCLASS()
class BRICK_API AMyVehicle : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyVehicle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Accelerate(float AxisValue);
	void Brake(float AxisValue);
	void SteerRight(float AxisValue);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* VehicleBody = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USpringArmComponent* SpringArmComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCameraComponent* CameraComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Suspension")
	USuspensionComponent* Suspension_FL;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Suspension")
	USuspensionComponent* Suspension_FR;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Suspension")
	USuspensionComponent* Suspension_RL;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Suspension")
	USuspensionComponent* Suspension_RR;
// 	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
// 	UMyVehicleMovementComponent* MovementComponent = nullptr;
};
