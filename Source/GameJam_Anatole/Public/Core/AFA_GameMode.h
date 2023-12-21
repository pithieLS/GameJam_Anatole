// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Logic/AFA_ToyVerifier.h"
#include "Actor/AFA_ToyPiece.h"
#include "AFA_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class GAMEJAM_ANATOLE_API AAFA_GameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AAFA_GameMode();

	virtual void BeginPlay() override;
};
