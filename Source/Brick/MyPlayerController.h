// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

class APawn;
/**
 * 
 */
UCLASS()
class BRICK_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();
	
protected:
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void Tick(float DeltaSeconds) override;
};
