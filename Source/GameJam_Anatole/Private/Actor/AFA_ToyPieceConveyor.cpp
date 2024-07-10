// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AFA_ToyPieceConveyor.h"
#include "Actor/AFA_ToyPiece.h"
#include "Core/GameModes/AFA_GameMode.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AAFA_ToyPieceConveyor::AAFA_ToyPieceConveyor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ToyPieceSpawnPoint = CreateDefaultSubobject<USceneComponent>("ToyPieceSpawnPoint");
	if (!ensure(ToyPieceSpawnPoint != nullptr))
		return;
	ToyPieceSpawnPoint->SetupAttachment(RootComponent);
}

void AAFA_ToyPieceConveyor::AddToyPieceToSpawn(TSubclassOf<class AAFA_ToyPiece> ToyPieceToAdd)
{
	AvailableToyPieces.Add(ToyPieceToAdd);

	SpawnedPiecesCount.Add(ToyPieceToAdd, 0);
}

// Called when the game starts or when spawned
void AAFA_ToyPieceConveyor::BeginPlay()
{
	Super::BeginPlay();
	
	AAFA_GameMode* GameMode = Cast<AAFA_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!ensure(GameMode != nullptr))
		return;

	// Bind delegates
	GameMode->OnGameStartedDelegate.AddUObject(this, &AAFA_ToyPieceConveyor::OnGameStartedHandler);
}

void AAFA_ToyPieceConveyor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AAFA_GameMode* GameMode = Cast<AAFA_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!ensure(GameMode != nullptr))
		return;

	// Unbind delegates
	GameMode->OnGameStartedDelegate.RemoveAll(this);

	Super::EndPlay(EndPlayReason);
}

TSubclassOf<class AAFA_ToyPiece> AAFA_ToyPieceConveyor::GetToyPieceToSpawn()
{
	float AverageSpawnCount = 0;
	int32 TotalSpawnCount = 0;
	for (TPair<TSubclassOf<class AAFA_ToyPiece>, int32> PieceSpawnCount : SpawnedPiecesCount)
		TotalSpawnCount += PieceSpawnCount.Value;
	AverageSpawnCount = (float)TotalSpawnCount / (float)SpawnedPiecesCount.Num();

	UE_LOG(LogTemp, Warning, TEXT("**************************************************************************************"));
	TArray<TSubclassOf<class AAFA_ToyPiece>> SpawnablePiecesArray; // Array that contains only the pieces that didn't spawned a lot more than the average spawn count of each pieces
	for (TPair<TSubclassOf<class AAFA_ToyPiece>, int32> PieceSpawnCount : SpawnedPiecesCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s //////////// %d"), *PieceSpawnCount.Key->GetDisplayNameText().ToString(), PieceSpawnCount.Value);

		if (PieceSpawnCount.Value < AverageSpawnCount - 2)
			return PieceSpawnCount.Key;
		else if (PieceSpawnCount.Value < AverageSpawnCount + 2)
			SpawnablePiecesArray.Add(PieceSpawnCount.Key);
	}
	UE_LOG(LogTemp, Warning, TEXT("average: %f"), AverageSpawnCount);
	UE_LOG(LogTemp, Warning, TEXT("**************************************************************************************"));
	int32 RandIndex = FMath::RandRange(0, SpawnablePiecesArray.Num() - 1);
	return SpawnablePiecesArray[RandIndex];
}

void AAFA_ToyPieceConveyor::SpawnToyPiece()
{
	TSubclassOf<class AAFA_ToyPiece> ToyPiecesToSpawn = GetToyPieceToSpawn();
	if (!ensure(ToyPiecesToSpawn != nullptr))
		return;

	AActor* SpawnedToyPiece = GetWorld()->SpawnActor<AActor>(ToyPiecesToSpawn, ToyPieceSpawnPoint->GetComponentTransform());
	if (!ensure(SpawnedToyPiece != nullptr))
		return;

	int32 SpawnedCount = *SpawnedPiecesCount.Find(ToyPiecesToSpawn);
	SpawnedPiecesCount.Add(ToyPiecesToSpawn, SpawnedCount + 1);
}

// Called every frame
void AAFA_ToyPieceConveyor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

