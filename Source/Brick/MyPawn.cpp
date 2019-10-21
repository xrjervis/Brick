// Fill out your copyright notice in the Description page of Project Settings.
#include "MyPawn.h"
#include "MyPawnMovementComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "UObject/ConstructorHelpers.h"

constexpr float CAMERA_BLEND_SECONDS = 0.75f;

// Sets default values
AMyPawn::AMyPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(30.f);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));
	RootComponent = SphereComponent;

	SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SphereVisual->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (SphereAsset.Succeeded()) {
		SphereVisual->SetStaticMesh(SphereAsset.Object);
		SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
		SphereVisual->SetWorldScale3D(FVector(0.8f));
	}

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MovementParticles"));
	ParticleSystemComponent->SetupAttachment(SphereVisual);
	ParticleSystemComponent->bAutoActivate = false;
	ParticleSystemComponent->SetRelativeLocation(FVector(-20.0f, 0.f, 20.0f));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/StarterContent/Particles/P_Fire.P_Fire"));
	if (ParticleAsset.Succeeded()) {
		ParticleSystemComponent->SetTemplate(ParticleAsset.Object);
	}

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraAttachmentArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->RelativeRotation = FRotator(-45.f, 0.f, 0.f);
	SpringArmComponent->TargetArmLength = 400.f;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 3.0f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	MovementComponent = CreateDefaultSubobject<UMyPawnMovementComponent>(TEXT("CustomMovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ZoomCamera(DeltaTime);
}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UPawnMovementComponent* AMyPawn::GetMovementComponent() const
{
	return MovementComponent;
}

void AMyPawn::FellOutOfWorld(const class UDamageType& dmgType)
{
	// Respawn player
	AActor* PlayerStart = GetWorld()->GetAuthGameMode()->FindPlayerStart(Controller);
	if (PlayerStart) {
		SetActorLocation(PlayerStart->GetActorLocation());
		SetActorRotation(PlayerStart->GetActorRotation());
		MovementComponent->StopMovementImmediately();
	}
}

void AMyPawn::MoveForward(float AxisValue)
{
	if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent)) {
		MovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
	}
}

void AMyPawn::MoveRight(float AxisValue)
{
	if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent)) {
		MovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
	}
}

void AMyPawn::Jump()
{
	MovementComponent->Jump();
}

void AMyPawn::CameraYaw(float AxisValue)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += AxisValue;
	SetActorRotation(NewRotation);
}

void AMyPawn::CameraPitch(float AxisValue)
{
	FRotator NewRotation = SpringArmComponent->GetComponentRotation();
	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + AxisValue, -80.0f, 80.0f);
	SpringArmComponent->SetWorldRotation(NewRotation);
}

void AMyPawn::ToggleParticle()
{
	if (ParticleSystemComponent && ParticleSystemComponent->Template) {
		ParticleSystemComponent->ToggleActive();
	}
}

void AMyPawn::SwitchCamera()
{
	bThirdPersonView = !bThirdPersonView;
	SpringArmComponent->bEnableCameraLag = bThirdPersonView ? true : false;
}

void AMyPawn::ZoomCamera(float DeltaTime)
{
	if (bThirdPersonView) {
		CameraZoomFactor += DeltaTime;
	}
	else {
		CameraZoomFactor -= DeltaTime;
	}
	CameraZoomFactor = FMath::Clamp(CameraZoomFactor, 0.f, 1.f);

	CameraComponent->FieldOfView = FMath::Lerp<float>(60.f, 90.f, CameraZoomFactor);
	SpringArmComponent->TargetArmLength = FMath::Lerp<float>(0.f, 400.f, CameraZoomFactor);
}