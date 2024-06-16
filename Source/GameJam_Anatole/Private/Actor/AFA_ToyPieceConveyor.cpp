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
	ToyPiecesToSpawn.Add(ToyPieceToAdd);
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

void AAFA_ToyPieceConveyor::SpawnToyPiece()
{
	int32 RandIndex = FMath::RandRange(0, ToyPiecesToSpawn.Num() - 1);

	AActor* SpawnedToyPiece = GetWorld()->SpawnActor<AActor>(ToyPiecesToSpawn[RandIndex], ToyPieceSpawnPoint->GetComponentTransform());
	if (!ensure(SpawnedToyPiece != nullptr))
		return;
}

// Called every frame
void AAFA_ToyPieceConveyor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

