// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/GameModes/AFA_GameMode.h"
#include "AFA_GameMode_MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class GAMEJAM_ANATOLE_API AAFA_GameMode_MainMenu : public AAFA_GameMode
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
