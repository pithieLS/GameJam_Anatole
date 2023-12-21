// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/AFA_MainWidget.h"
#include "Core/AFA_GameMode.h"
#include "Components/TextBlock.h"
#include <Kismet/GameplayStatics.h>

void UAFA_MainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AAFA_GameMode* GameMode = Cast<AAFA_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!ensure(GameMode != nullptr))
		return;

	GameMode->OnScoreChanged.AddUObject(this, &UAFA_MainWidget::UpdateScoreText);
	UpdateScoreText();
}

void UAFA_MainWidget::UpdateScoreText()
{
	AAFA_GameMode* GameMode = Cast<AAFA_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!ensure(GameMode != nullptr))
		return;

	const int32 Score = GameMode->GetScore();
	ScoreText->SetText(FText::AsNumber(Score));
}
