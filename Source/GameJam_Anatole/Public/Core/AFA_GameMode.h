// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Logic/AFA_ToyOrder.h"
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

	UFUNCTION(BlueprintCallable)
	void StartGame();
	UFUNCTION(BlueprintCallable)
	void StopGame();

	// Delegates
	DECLARE_MULTICAST_DELEGATE(FOnGameStarted)
	FOnGameStarted OnGameStartedDelegate;
	DECLARE_MULTICAST_DELEGATE(FOnScoreChanged)
	FOnScoreChanged OnScoreChangedDelegate;
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnOrderAddedDelegate, UAFA_ToyOrder* NewOrder)
	FOnOrderAddedDelegate OnOrderAddedDelegate;
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnToyVerified, bool bIsValid)
	FOnToyVerified OnToyVerifiedDelegate;

	// Getters
	int32 GetScore() { return Score; }
	bool GetIsGameRunning() { return bIsGameStarted; }
	TArray<UAFA_ToyOrder*> GetCurrentOrders() { return CurrentOrders; }

	// Orders related
	void AddNewOrder(TSubclassOf<UAFA_ToyOrder> NewOrderClass);
	void RemoveOrder(UAFA_ToyOrder* OrderToRemove); //{ CurrentOrders.Remove(OrderToRemove); }
	void AddToScore(int32 ScoreToAdd);

	// Countdown related
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StartCountdown(); // Countdown before the game starts. Usually called on the BeginPlay of the Level BP

	// Order related
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<UAFA_ToyOrder*> CurrentOrders;

	// Countdown related
	UPROPERTY(BlueprintReadWrite)
	float StartCountdownTimeLeft = 3;

protected:
	// Score related
	int32 Score = 0;

	UPROPERTY(BlueprintReadWrite)
	bool bIsGameStarted = false;
};
