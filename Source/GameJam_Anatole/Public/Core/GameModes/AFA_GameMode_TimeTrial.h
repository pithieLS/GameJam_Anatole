// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/GameModes/AFA_GameMode.h"
#include "AFA_GameMode_TimeTrial.generated.h"

//TODO: Timer widget + gameflow + what happen WHEN game ends

/**
 * 
 */
UCLASS()
class GAMEJAM_ANATOLE_API AAFA_GameMode_TimeTrial : public AAFA_GameMode
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void StopGame() override;

	// Getters
	float GetTimerValue() { return TimerValue; }

protected:

	// Timer related func
	UFUNCTION(BlueprintCallable)
	void StartTimer();
	UFUNCTION(BlueprintCallable)
	void OnTimerEnd();

	// Timer related var
	UPROPERTY(EditDefaultsOnly, Category = "Time Trial Properties")
	float TimerMax;
	float TimerValue;
	bool isTimerStarted = false;
};
