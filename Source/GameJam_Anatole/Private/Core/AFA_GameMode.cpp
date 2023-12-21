// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/AFA_GameMode.h"
#include "Pawn/AFA_PawnMechanicalArm.h"
#include <Kismet/GameplayStatics.h>

AAFA_GameMode::AAFA_GameMode()
{
	PlayerControllerClass = AAFA_GameMode::StaticClass();
}

void AAFA_GameMode::BeginPlay()
{
	Super::BeginPlay();
	//Create PlayerController for second player;
	APlayerController* GamepadController = UGameplayStatics::CreatePlayer(this, -1, true);
}
