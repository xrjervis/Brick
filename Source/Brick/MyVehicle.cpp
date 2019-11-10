// Fill out your copyright notice in the Description page of Project Settings.
#include "MyVehicle.h"
#include "MyVehicleMovementComponent.h"
#include "SuspensionComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AMyVehicle::AMyVehicle()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

// 	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
// 	BoxComponent->SetBoxExtent(FVector(160.f, 130.f, 32.f));
// 	BoxComponent->SetSimulatePhysics(true);
// 	BoxComponent->SetMassOverrideInKg();
// 	BoxComponent->SetCollisionProfileName(TEXT("Pawn"));
// 	RootComponent = BoxComponent;

	VehicleBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	VehicleBody->SetSimulatePhysics(true);
	VehicleBody->SetMassOverrideInKg();
	VehicleBody->SetCollisionProfileName(TEXT("Pawn"));
	RootComponent = VehicleBody;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraAttachmentArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->RelativeRotation = FRotator(-45.f, 0.f, 0.f);
	SpringArmComponent->TargetArmLength = 400.f;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 10.0f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	Suspension_FL = CreateDefaultSubobject<USuspensionComponent>(TEXT("Suspension_FL"));
	Suspension_FR = CreateDefaultSubobject<USuspensionComponent>(TEXT("Suspension_FR"));
	Suspension_RL = CreateDefaultSubobject<USuspensionComponent>(TEXT("Suspension_RL"));
	Suspension_RR = CreateDefaultSubobject<USuspensionComponent>(TEXT("Suspension_RR"));

	Suspension_FL->SetupAttachment(BoxComponent);
	Suspension_FR->SetupAttachment(BoxComponent);
	Suspension_RL->SetupAttachment(BoxComponent);
	Suspension_RR->SetupAttachment(BoxComponent);
// 	MovementComponent = CreateDefaultSubobject<UMyVehicleMovementComponent>(TEXT("VehicleMovementComponent"));
// 	MovementComponent->UpdatedComponent = RootComponent;
}

// Called when the game starts or when spawned
void AMyVehicle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyVehicle::Accelerate(float AxisValue)
{
	//MovementComponent->Accelerate(AxisValue);
}

void AMyVehicle::Brake(float AxisValue)
{
	//MovementComponent->Brake(AxisValue);
}

void AMyVehicle::SteerRight(float AxisValue)
{
	//MovementComponent->SteerRight(AxisValue);
}
