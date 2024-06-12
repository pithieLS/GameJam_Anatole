// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AFA_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GAMEJAM_ANATOLE_API UAFA_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	int32 PlayerNumber = 0;
};
