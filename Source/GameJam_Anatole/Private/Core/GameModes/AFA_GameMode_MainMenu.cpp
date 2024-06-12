// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameModes/AFA_GameMode_MainMenu.h"
#include <Kismet/GameplayStatics.h>

void AAFA_GameMode_MainMenu::BeginPlay()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!ensure(PlayerController != nullptr))
		return;

	PlayerController->SetInputMode(FInputModeUIOnly());
	PlayerController->bShowMouseCursor = true;
}

void AAFA_GameMode_MainMenu::Tick(float DeltaTime)
{

}
