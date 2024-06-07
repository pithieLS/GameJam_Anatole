// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Logic/AFA_ToyOrder.h"
#include "AFA_GameMode.generated.h"

class AAFA_ToyPiece;

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
	virtual void Tick(float DeltaTime) override;

	// Gameflow functions
	UFUNCTION(BlueprintCallable)
	void StartGame();
	UFUNCTION(BlueprintCallable)
	virtual void StopGame();
	UFUNCTION(BlueprintNativeEvent)
	void OnGameEndedHandler();

	// Delegates
	DECLARE_MULTICAST_DELEGATE(FOnGameStarted)
	FOnGameStarted OnGameStartedDelegate;
	DECLARE_MULTICAST_DELEGATE(FOnGameEnded)
	FOnGameEnded OnGameEndedDelegate;
	DECLARE_MULTICAST_DELEGATE(FOnScoreChanged)
	FOnScoreChanged OnScoreChangedDelegate;
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnOrderAddedDelegate, UAFA_ToyOrder* NewOrder)
	FOnOrderAddedDelegate OnOrderAddedDelegate;
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnToyVerified, bool bIsValid)
	FOnToyVerified OnToyVerifiedDelegate;

	// Getters
	int32 GetScore() { return Score; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsGameRunning() { return bIsGameRunning; }
	TArray<UAFA_ToyOrder*> GetCurrentOrders() { return CurrentOrders; }

	// Verification related
	void VerifyOverlappedToy(AAFA_ToyPiece* InToyPiece);
	void OnToyVerified(UAFA_ToyOrder* VerifiedOrder, bool bIsValid);
	UFUNCTION(BlueprintCallable)
	void MakeNewOrder();

	// Orders related
	void HandleOrdersCreation(float DeltaTime);
	void AddNewOrder(TSubclassOf<UAFA_ToyOrder> NewOrderClass);
	void RemoveOrder(UAFA_ToyOrder* OrderToRemove); //{ CurrentOrders.Remove(OrderToRemove); }
	void DecrementOrdersLifetime(float DeltaTime);
	void AddToScore(int32 ScoreToAdd);

	// Countdown related
	UFUNCTION(BlueprintCallable)
	void StartCountdown(); // Countdown before the game starts. Usually called on the BeginPlay of the Level BP

	// Countdown related
	UPROPERTY(BlueprintReadWrite)
	float StartCountdownTimeLeft = 3;
	UPROPERTY(EditDefaultsOnly)
	class TSubclassOf<UUserWidget> CountdownWidgetClassBP;
	class UUserWidget* CountdownWidget;
	bool bIsStartCountdownStarted = false;

protected:
	// Score related
	int32 Score = 0;

	// Gameflow related
	UPROPERTY(BlueprintReadWrite)
	bool bIsGameRunning = false;

	// Order related
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order/Verification")
	TArray<TSubclassOf<UAFA_ToyOrder>> AvailableOrders;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<UAFA_ToyOrder>, int32> OrderVerificationCount;
	UPROPERTY(BlueprintReadOnly)
	TArray<UAFA_ToyOrder*> CurrentOrders;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order/Verification")
	float NewOrderDelay = 45;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order/Verification")
	int32 MaxOderNumber = 5;
	float LastOrderTimePassed = NewOrderDelay;

	// Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	TSubclassOf<UUserWidget> HUDWidgetClassBP;

};
