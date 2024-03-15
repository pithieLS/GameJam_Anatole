// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AFA_ScoreWidgetOverlay.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class GAMEJAM_ANATOLE_API UAFA_ScoreWidgetOverlay : public UUserWidget
{
	GENERATED_BODY()
public:

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void UpdateScoreText();

	// Components
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreText;
};
