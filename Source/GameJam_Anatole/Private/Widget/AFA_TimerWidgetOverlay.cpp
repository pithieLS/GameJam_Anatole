// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/AFA_TimerWidgetOverlay.h"
#include "Kismet/GameplayStatics.h"
#include "Core/GameModes/AFA_GameMode_TimeTrial.h"
#include "Components/TextBlock.h"


void UAFA_TimerWidgetOverlay::NativeConstruct()
{
	GameModeTimeTrial = Cast<AAFA_GameMode_TimeTrial>(UGameplayStatics::GetGameMode(this));
	if (!ensure(GameModeTimeTrial != nullptr))
		return;
}

void UAFA_TimerWidgetOverlay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	CountdownText->SetText(FText::AsNumber(GameModeTimeTrial->GetTimerValue()));
}
