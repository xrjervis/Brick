// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UCameraComponent;
class UMyPawnMovementComponent;
class UParticleSystemComponent;
class USpringArmComponent;

UCLASS()
class BRICK_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void						BeginPlay() override;

public:	
	// Called every frame
	virtual void						Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void						SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UPawnMovementComponent*		GetMovementComponent() const override;

	virtual void						FellOutOfWorld(const class UDamageType& dmgType) override;

	void								MoveForward(float AxisValue);
	void								MoveRight(float AxisValue);
	void								CameraYaw(float AxisValue);
	void								CameraPitch(float AxisValue);
	void								ToggleParticle();
	void								SwitchCamera();
	void								ZoomCamera(float DeltaTime);
	void								Jump();


public:
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadWrite)
	USphereComponent*					SphereComponent = nullptr;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent*				SphereVisual = nullptr;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadWrite)
	UMyPawnMovementComponent*			MovementComponent = nullptr;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadWrite)
	UParticleSystemComponent*			ParticleSystemComponent = nullptr;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadWrite)
	USpringArmComponent*				SpringArmComponent = nullptr;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadWrite)
	UCameraComponent*					CameraComponent = nullptr;

	bool								bThirdPersonView = true;
	float								CameraZoomFactor = 1.f; // [0.0f, 1.0f]
};