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
	APlayerController* NewController = UGameplayStatics::CreatePlayer(this, -1, true);
}

void AAFA_GameMode::StartGame()
{
	bIsGameStarted = true;
	OnGameStartedDelegate.Broadcast();
}

void AAFA_GameMode::StopGame()
{
	bIsGameStarted = false;
	// TODO
}

void AAFA_GameMode::AddNewOrder(TSubclassOf<UAFA_ToyOrder> NewOrderClass)
{
	UAFA_ToyOrder* SpawnedNewOrder = NewObject<UAFA_ToyOrder>(this, NewOrderClass);
	if (!ensure(SpawnedNewOrder != nullptr))
		return;

	SpawnedNewOrder->InitialiseOrder();

	CurrentOrders.Add(SpawnedNewOrder);

	OnOrderAddedDelegate.Broadcast(SpawnedNewOrder);
}

void AAFA_GameMode::RemoveOrder(UAFA_ToyOrder* OrderToRemove)
{
	CurrentOrders.Remove(OrderToRemove);
	OrderToRemove->ConditionalBeginDestroy();

	//OnOrdersChangedDelegate.Broadcast();
}

void AAFA_GameMode::AddToScore(int32 ScoreToAdd)
{
	Score += ScoreToAdd;
	Score = Score < 0 ? 0 : Score;

	OnScoreChangedDelegate.Broadcast();
}
