// Fill out your copyright notice in the Description page of Project Settings.
#include "MyVehicle.h"
#include "MyVehicleMovementComponent.h"

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

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	BoxComponent->SetBoxExtent(FVector(160.f, 130.f, 32.f));
	BoxComponent->SetSimulatePhysics(true);
	BoxComponent->SetMassOverrideInKg();
	BoxComponent->SetCollisionProfileName(TEXT("Pawn"));
	RootComponent = BoxComponent;

	BoxVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	BoxVisual->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/EngineMeshes/Cube.Cube"));
	if (CubeAsset.Succeeded()) {
		BoxVisual->SetStaticMesh(CubeAsset.Object);
		BoxVisual->SetWorldScale3D(FVector(1.2f, 1.0f, 0.2f));
	}

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraAttachmentArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->RelativeRotation = FRotator(-45.f, 0.f, 0.f);
	SpringArmComponent->TargetArmLength = 400.f;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 10.0f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	MovementComponent = CreateDefaultSubobject<UMyVehicleMovementComponent>(TEXT("VehicleMovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
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

// Called to bind functionality to input
void AMyVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMyVehicle::Accelerate(float AxisValue)
{
	MovementComponent->Accelerate(AxisValue);
}

void AMyVehicle::Brake(float AxisValue)
{
	MovementComponent->Brake(AxisValue);
}

void AMyVehicle::SteerRight(float AxisValue)
{
	MovementComponent->SteerRight(AxisValue);
}
