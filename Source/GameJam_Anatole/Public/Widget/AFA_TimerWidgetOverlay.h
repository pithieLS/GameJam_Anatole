// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AFA_TimerWidgetOverlay.generated.h"

/**
 * 
 */


UCLASS()
class GAMEJAM_ANATOLE_API UAFA_TimerWidgetOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:

	/* If theres the need to implement this widget in other GameMode make a child class of GameMode_TimeTrail
	   and override functions */
	class AAFA_GameMode_TimeTrial* GameModeTimeTrial; 

	// Components
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CountdownText;
};
