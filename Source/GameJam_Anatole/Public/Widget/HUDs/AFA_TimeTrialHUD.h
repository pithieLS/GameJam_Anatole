// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AFA_TimeTrialHUD.generated.h"

/**
 * 
 */
UCLASS()
class GAMEJAM_ANATOLE_API UAFA_TimeTrialHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:

protected:

	// Components 
	UPROPERTY(meta = (BindWidget))
	class UAFA_TimerWidgetOverlay* TimerOverlay;
	UPROPERTY(meta = (BindWidget))
	class UAFA_ScoreWidgetOverlay* ScoreOverlay;
	UPROPERTY(meta = (BindWidget))
	class UAFA_OrdersListWidget* OrdersListOverlay;
};
