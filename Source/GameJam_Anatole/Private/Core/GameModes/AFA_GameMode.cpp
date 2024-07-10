// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameModes/AFA_GameMode.h"
#include "Pawn/AFA_PawnMechanicalArm.h"
#include <Kismet/GameplayStatics.h>
#include "Actor/AFA_ToyRTActor.h"
#include "Actor/AFA_ToyPiece.h"
#include <Blueprint/UserWidget.h>
#include "Core/AFA_GameInstance.h"
#include "Core/AFA_LevelActorManager.h"
#include "Actor/AFA_ToyPieceConveyor.h"

AAFA_GameMode::AAFA_GameMode()
{
}

void AAFA_GameMode::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	// Ensure that number of player has been properly set
	UAFA_GameInstance* GameInstance = Cast<UAFA_GameInstance>(GetWorld()->GetGameInstance());
	if (!ensure(GameInstance != nullptr))
		return;
	if(GameInstance->PlayerNumber == 0)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("(!) Please use CustomOpenLevel from the GameMode and properly set the number of players (!)"));

	ApplyPlayerNumberChanges();

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

	//Bind delegated
	OnGameEndedDelegate.AddUObject(this, &AAFA_GameMode::OnGameEndedHandler);
}

void AAFA_GameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnGameEndedDelegate.RemoveAll(this);

	Super::EndPlay(EndPlayReason);
}

void AAFA_GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Order related
	if (bIsGameRunning)
	{
		HandleOrdersCreation(DeltaTime);
		DecrementOrdersLifetime(DeltaTime);
	}

	if (bIsStartCountdownStarted && bIsGameRunning == false)
	{
		StartCountdownTimeLeft -= DeltaTime;

		if (StartCountdownTimeLeft <= 0) // Where game starts
		{
			bIsStartCountdownStarted = false;

			StartGame();
			CountdownWidget->RemoveFromParent();

		}
	}
}

void AAFA_GameMode::CustomOpenLevel(FString InLevelName, int32 InPlayerNumber)
{
	UAFA_GameInstance* GameInstance = Cast<UAFA_GameInstance>(GetWorld()->GetGameInstance());
	if (!ensure(GameInstance != nullptr))
		return;
	GameInstance->PlayerNumber = InPlayerNumber;

	UGameplayStatics::OpenLevel(this, FName(InLevelName));
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
		int32 OrderVerifiedCountNb = 0;

		if(OrderVerificationCount.Contains(VerifiedOrder->GetClass()))
			OrderVerifiedCountNb = *OrderVerificationCount.Find(VerifiedOrder->GetClass());

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
		if(CurrentOrders.Num() < MaxOderNumber)
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

void AAFA_GameMode::ApplyPlayerNumberChanges()
{
	// Make timer because actors are not instantiated when gamemode's BeginPlay() is executed
	FTimerHandle ApplyPlayerNumberChangesHandler;
	GetWorld()->GetTimerManager().SetTimer(ApplyPlayerNumberChangesHandler, [this, &ApplyPlayerNumberChangesHandler]() {
		AAFA_LevelActorManager* LevelActorManager = Cast<AAFA_LevelActorManager>(UGameplayStatics::GetActorOfClass(this, AAFA_LevelActorManager::StaticClass()));
		if (!ensure(LevelActorManager != nullptr))
			return;
		TArray<AAFA_PawnMechanicalArm*>& _PlayersMechanicalArmPawn = LevelActorManager->PlayersMechanicalArmPawn;
		TArray<AAFA_ToyPieceConveyor*>& _PlayersToyPieceConveyor = LevelActorManager->PlayersToyPieceConveyor;

		UAFA_GameInstance* GameInstance = Cast<UAFA_GameInstance>(GetGameInstance());
		if (!ensure(GameInstance != nullptr))
			return;
		int32 PlayerNumber = GameInstance->PlayerNumber;

		if (PlayerNumber <= 0)
			return;

		// Destroy all unused mechanical arms and remove it from the OG array
		for (int32 PawnIndex = PlayerNumber; PawnIndex < _PlayersMechanicalArmPawn.Num(); PawnIndex++)
			_PlayersMechanicalArmPawn[PawnIndex]->Destroy();
		_PlayersMechanicalArmPawn.SetNum(PlayerNumber);

		// Destroy all unused conveyor and remove it from the OG array
		for (int32 PawnIndex = PlayerNumber; PawnIndex < _PlayersToyPieceConveyor.Num(); PawnIndex++)
			_PlayersToyPieceConveyor[PawnIndex]->Destroy();
		_PlayersToyPieceConveyor.SetNum(PlayerNumber);

		// Populate arrays of toy pieces of each conveyor
		TArray<TArray<TSubclassOf<AAFA_ToyPiece>>> AllOrdersToyPieces;
		int32 OrderIndex = 0;
		// Add all associated toy pieces for each order in a double array
		for (TSubclassOf<UAFA_ToyOrder> OrderSubClass : AvailableOrders)
		{
			UAFA_ToyOrder* OrderCDO = Cast<UAFA_ToyOrder>(OrderSubClass->GetDefaultObject());
			TArray<TSubclassOf<AAFA_ToyPiece>> ToyPiecesClassOfOrder;

			AllOrdersToyPieces.Add({});

			// For each order, add the toy pieces that are in the verifications.
			for (FPieceVerification PieceVerif : OrderCDO->PiecesVerifications)
			{
				AllOrdersToyPieces[OrderIndex].AddUnique(PieceVerif.VerificatedToyPiece);

				for (TPair<FName, TSubclassOf<AAFA_ToyPiece>>& AttachedToyPiece : PieceVerif.AttachedToyPieces)
					AllOrdersToyPieces[OrderIndex].AddUnique(AttachedToyPiece.Value);
			}

			OrderIndex++;
		}

		// Add toy pieces to each conveyor
		int32 ConveyorIndexMax = PlayerNumber - 1;
		int32 ConveyorIndex = 0;
		for (TArray<TSubclassOf<AAFA_ToyPiece>> OrderToPieces : AllOrdersToyPieces)
		{
			for (TSubclassOf<AAFA_ToyPiece> ToyPiece : OrderToPieces)
			{
				LevelActorManager->AddToyPieceToConveyor(ConveyorIndex, ToyPiece);

				if (ConveyorIndex >= ConveyorIndexMax)
					ConveyorIndex = 0;
				else
					ConveyorIndex++;
			}
		}

		// If sp, increase player's claw's move range
		if (PlayerNumber == 1)
		{
			AAFA_PawnMechanicalArm* PlayerMechArm = LevelActorManager->PlayersMechanicalArmPawn[0];
			if (!ensure(PlayerMechArm != nullptr))
				return;
			PlayerMechArm->SetMaxRight(PlayerMechArm->GetMaxRight() * 1.5f);
		}

		GetWorld()->GetTimerManager().ClearTimer(ApplyPlayerNumberChangesHandler);
		}, 0.05f, false);
}
