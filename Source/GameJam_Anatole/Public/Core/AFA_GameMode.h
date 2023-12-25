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

	// Delegates
	DECLARE_MULTICAST_DELEGATE(FOnGameStarted)
	FOnGameStarted OnGameStarted;
	DECLARE_MULTICAST_DELEGATE(FOnScoreChanged)
	FOnScoreChanged OnScoreChanged;
	DECLARE_MULTICAST_DELEGATE(FOnOrdersChanged)
	FOnOrdersChanged OnOrdersChanged;
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnToyVerified, bool bIsValid)
	FOnToyVerified OnToyVerified;

	// Getters
	int32 GetScore() { return Score; }
	bool GetIsGameRunning() { return bIsGameRunning; }
	TMap<TSubclassOf<UAFA_ToyVerifier>, float>& GetCurrentOrders() { return CurrentOrdersToLifeTime; }

	void AddToScore(int32 ScoreToAdd);

	// Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UAFA_ToyVerifier>> AvailableVerifiers;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<TSubclassOf<UAFA_ToyVerifier>, float> CurrentOrdersToLifeTime;

protected:
	// Score related
	int32 Score = 0;

	UPROPERTY(BlueprintReadWrite)
	bool bIsGameRunning = false;
};
