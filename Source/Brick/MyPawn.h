// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

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
	UMyPawnMovementComponent*			MovementComponent = nullptr;
	UParticleSystemComponent*			ParticleSystemComponent = nullptr;
	USpringArmComponent*				SpringArmComponent = nullptr;
	UCameraComponent*					CameraComponent = nullptr;
	bool								bThirdPersonView = true;
	float								CameraZoomFactor = 1.f; // [0.0f, 1.0f]
};
