// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/AFA_ScoreWidgetOverlay.h"
#include "Core/GameModes/AFA_GameMode.h"
#include "Components/TextBlock.h"
#include <Kismet/GameplayStatics.h>

void UAFA_ScoreWidgetOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	AAFA_GameMode* GameMode = Cast<AAFA_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!ensure(GameMode != nullptr))
		return;

	GameMode->OnScoreChangedDelegate.AddUObject(this, &UAFA_ScoreWidgetOverlay::UpdateScoreText);
	UpdateScoreText();
}

void UAFA_ScoreWidgetOverlay::NativeDestruct()
{
	AAFA_GameMode* GameMode = Cast<AAFA_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!ensure(GameMode != nullptr))
		return;

	GameMode->OnScoreChangedDelegate.RemoveAll(this);

	Super::NativeDestruct();
}

void UAFA_ScoreWidgetOverlay::UpdateScoreText()
{
	AAFA_GameMode* GameMode = Cast<AAFA_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!ensure(GameMode != nullptr))
		return;

	const int32 Score = GameMode->GetScore();
	ScoreText->SetText(FText::AsNumber(Score));
}
