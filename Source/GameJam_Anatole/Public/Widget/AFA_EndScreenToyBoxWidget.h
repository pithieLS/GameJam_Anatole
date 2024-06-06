// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AFA_EndScreenToyBoxWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMEJAM_ANATOLE_API UAFA_EndScreenToyBoxWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void UpdateWidget(AAFA_GameMode* GameMode);

protected:

	//Components
	UPROPERTY(meta = (BindWidget))
	class UImage* ToyBoxRenderTarget;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ScoreText;

	UPROPERTY(meta = (BindWidget))
	class UButton* RetryButton;
};
