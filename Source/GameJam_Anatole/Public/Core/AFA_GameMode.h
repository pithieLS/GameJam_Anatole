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

	// Delegates
	DECLARE_MULTICAST_DELEGATE(FOnGameStarted)
	FOnGameStarted OnGameStartedDelegate;
	DECLARE_MULTICAST_DELEGATE(FOnScoreChanged)
	FOnScoreChanged OnScoreChangedDelegate;
	DECLARE_MULTICAST_DELEGATE(FOnOrdersChanged)
	FOnOrdersChanged OnOrdersChangedDelegate;
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnToyVerified, bool bIsValid)
	FOnToyVerified OnToyVerifiedDelegate;

	// Getters
	int32 GetScore() { return Score; }
	bool GetIsGameRunning() { return bIsGameRunning; }
	TArray<UAFA_ToyOrder*>& GetCurrentOrders() { return CurrentOrders; }

	void AddNewOrder(UAFA_ToyOrder* NewOrder); //{  }
	void RemoveOrder(UAFA_ToyOrder* OrderToRemove); //{ CurrentOrders.Remove(OrderToRemove); }
	void AddToScore(int32 ScoreToAdd);

	// Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UAFA_ToyOrder>> AvailableOrders;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<UAFA_ToyOrder*> CurrentOrders;

protected:
	// Score related
	int32 Score = 0;

	UPROPERTY(BlueprintReadWrite)
	bool bIsGameRunning = false;
};
