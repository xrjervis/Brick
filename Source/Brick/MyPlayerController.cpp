// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MyPawn.h"
#include "MyVehicle.h"

AMyPlayerController::AMyPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void AMyPlayerController::OnPossess(APawn* aPawn)
{
	InputComponent->ClearActionBindings();
	InputComponent->AxisBindings.Empty();

	if (AMyPawn* MyPawn = Cast<AMyPawn>(aPawn)) {
		InputComponent->BindAction("ToggleParticle", IE_Pressed, MyPawn, &AMyPawn::ToggleParticle);
		InputComponent->BindAction("SwitchCamera", IE_Pressed, MyPawn, &AMyPawn::SwitchCamera);
		InputComponent->BindAction("Jump", IE_Pressed, MyPawn, &AMyPawn::Jump);
		InputComponent->BindAxis("MoveForward", MyPawn, &AMyPawn::MoveForward);
		InputComponent->BindAxis("MoveRight", MyPawn, &AMyPawn::MoveRight);
		InputComponent->BindAxis("CameraYaw", MyPawn, &AMyPawn::CameraYaw);
		InputComponent->BindAxis("CameraPitch", MyPawn, &AMyPawn::CameraPitch);
	}
	else if (AMyVehicle * MyVehicle = Cast<AMyVehicle>(aPawn)) {
		InputComponent->BindAxis("Accelerate", MyVehicle, &AMyVehicle::Accelerate);
		InputComponent->BindAxis("Brake", MyVehicle, &AMyVehicle::Brake);
		InputComponent->BindAxis("SteerRight", MyVehicle, &AMyVehicle::SteerRight);
	}

	Super::OnPossess(aPawn);
}

void AMyPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
