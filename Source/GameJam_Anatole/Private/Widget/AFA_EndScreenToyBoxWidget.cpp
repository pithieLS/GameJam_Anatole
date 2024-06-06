// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/AFA_EndScreenToyBoxWidget.h"
#include <Kismet/GameplayStatics.h>
#include "Core/GameModes/AFA_GameMode.h"
#include <Components/TextBlock.h>

void UAFA_EndScreenToyBoxWidget::UpdateWidget(AAFA_GameMode* GameMode)
{
	ScoreText->SetText(FText::FromString(FString::FromInt((GameMode->GetScore()))));
}
