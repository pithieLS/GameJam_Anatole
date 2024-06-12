// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameModes/AFA_GameMode.h"
#include "Pawn/AFA_PawnMechanicalArm.h"
#include <Kismet/GameplayStatics.h>
#include "Actor/AFA_ToyRTActor.h"
#include "Actor/AFA_ToyPiece.h"
#include <Blueprint/UserWidget.h>

AAFA_GameMode::AAFA_GameMode()
{
	PlayerControllerClass = AAFA_GameMode::StaticClass();
}

void AAFA_GameMode::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	//Create PlayerController for second player;
	APlayerController* NewController = UGameplayStatics::CreatePlayer(this, -1, true);

	// Spawns all overlays actors for all available orders
	FVector ToyOverlaySpawnLocation = FVector(0, 0, -500);
	for (TSubclassOf<UAFA_ToyOrder> ToyOrder : AvailableOrders)
	{
		UAFA_ToyOrder* ToyOrderCDO = Cast<UAFA_ToyOrder>(ToyOrder->GetDefaultObject());
		if (!ensure(ToyOrderCDO != nullptr))
			return;

		AAFA_ToyRTActor* SpawnedOverlayActor = GetWorld()->SpawnActor<AAFA_ToyRTActor>(ToyOrderCDO->ToyOverlayActor, ToyOverlaySpawnLocation, FRotator::ZeroRotator);
		if (!ensure(SpawnedOverlayActor != nullptr))
			return;

		ToyOverlaySpawnLocation.Z -= 150;
	}

	// Create HUD
	UUserWidget* HUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClassBP);
	if (!ensure(HUDWidget != nullptr))
		return;

	HUDWidget->AddToViewport();

	//// Add available order to the map that count the number of times an order has been verificated
	//OrderVerificationCount.Empty();
	//for (TSubclassOf<UAFA_ToyOrder> _Order : AvailableOrders)
	//	OrderVerificationCount.Add(_Order, 0);

	//Bind delegated
	OnGameEndedDelegate.AddUObject(this, &AAFA_GameMode::OnGameEndedHandler);
}

void AAFA_GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Order related
	if (bIsGameRunning)
	{
		DecrementOrdersLifetime(DeltaTime);
		HandleOrdersCreation(DeltaTime);
	}

	if (bIsStartCountdownStarted && bIsGameRunning == false)
	{
		StartCountdownTimeLeft -= DeltaTime;

		if (StartCountdownTimeLeft <= 0) // Where game starts
		{
			bIsStartCountdownStarted = false;

			StartGame();
			CountdownWidget->ConditionalBeginDestroy();

		}
	}
}

void AAFA_GameMode::StartGame()
{
	bIsGameRunning = true;
	OnGameStartedDelegate.Broadcast();
}

void AAFA_GameMode::StopGame()
{
	bIsGameRunning = false;
	OnGameEndedDelegate.Broadcast();
	// Configure in child class
}

void AAFA_GameMode::OnGameEndedHandler_Implementation()
{
	if(!ensure(GEngine != nullptr))
		return;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("(!) Please override the OnGameEndedHandler event in this level gamemode's bp class (!)"));
}

void AAFA_GameMode::VerifyOverlappedToy(AAFA_ToyPiece* InToyPiece)
{
	if (!bIsGameRunning)
		return;

	bool bIsAnyToyValid = false;

	UAFA_ToyOrder* VerifiedOrder = nullptr;
	for (UAFA_ToyOrder* _Order : CurrentOrders)
	{
		if (_Order->VerifyToy(InToyPiece))
		{
			// TODO: make the order with the less lifetime left be the verified order
			bIsAnyToyValid = true;
			VerifiedOrder = _Order;
			break;
		}
	}

	InToyPiece->DestroyToyGroup();
	OnToyVerified(VerifiedOrder, bIsAnyToyValid);
}

void AAFA_GameMode::OnToyVerified(UAFA_ToyOrder* VerifiedOrder, bool bIsValid)
{
	const int32 ScoreToAdd = bIsValid ? 1 : -1;
	AddToScore(ScoreToAdd);
	if(bIsValid)
	{
		int32 OrderVerifiedCountNb = *OrderVerificationCount.Find(VerifiedOrder->GetClass());
		OrderVerificationCount.Add(VerifiedOrder->GetClass(), OrderVerifiedCountNb + 1);
	}

	OnToyVerifiedDelegate.Broadcast(bIsValid);

	if (bIsValid)
		RemoveOrder(VerifiedOrder);

}

void AAFA_GameMode::MakeNewOrder()
{
	const int32 RandIndex = FMath::RandRange(0, AvailableOrders.Num() - 1);

	AddNewOrder(AvailableOrders[RandIndex]);
}

void AAFA_GameMode::HandleOrdersCreation(float DeltaTime)
{
	if (LastOrderTimePassed >= NewOrderDelay)
	{
		MakeNewOrder();
		LastOrderTimePassed = 0;
	}

	LastOrderTimePassed += DeltaTime;
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
}

void AAFA_GameMode::DecrementOrdersLifetime(float DeltaTime)
{
	for (int32 OrderIndex = 0; OrderIndex < CurrentOrders.Num(); OrderIndex++)
	{
		UAFA_ToyOrder* _Order = CurrentOrders[OrderIndex];
		if(!_Order)
			continue;

		CurrentOrders[OrderIndex]->DecrementLifetime(DeltaTime);
	}
}

void AAFA_GameMode::AddToScore(int32 ScoreToAdd)
{
	Score += ScoreToAdd;
	Score = Score < 0 ? 0 : Score;

	OnScoreChangedDelegate.Broadcast();
}

void AAFA_GameMode::StartCountdown()
{
	CountdownWidget = CreateWidget<UUserWidget>(GetWorld(), CountdownWidgetClassBP); // Create countdown widget
	CountdownWidget->AddToViewport();

	bIsStartCountdownStarted = true;
}
