// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AFA_ToyPieceConveyor.h"
#include "Actor/AFA_ToyPiece.h"

// Sets default values
AAFA_ToyPieceConveyor::AAFA_ToyPieceConveyor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAFA_ToyPieceConveyor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAFA_ToyPieceConveyor::SpawnToyPiece()
{
	int32 RandIndex = FMath::RandRange(0, ToyPiecesToSpawn.Num() - 1);
	TSubclassOf<AAFA_ToyPiece> PieceClassToSpawn;

	AActor* SpawnedToyPiece = GetWorld()->SpawnActor<AActor>(PieceClassToSpawn, ToyPieceSpawnPoint->GetComponentTransform());
}

// Called every frame
void AAFA_ToyPieceConveyor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

