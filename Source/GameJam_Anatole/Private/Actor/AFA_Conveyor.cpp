// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AFA_Conveyor.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Actor/AFA_ToyPiece.h"

AAFA_Conveyor::AAFA_Conveyor()
{
	PrimaryActorTick.bCanEverTick = true;

	ConvoyerSceneComp = CreateDefaultSubobject<USceneComponent>("ConvoyerSceneComp");
	if (!ensure(ConvoyerSceneComp != nullptr))
		return;
	SetRootComponent(ConvoyerSceneComp);

	ConvoyerBelt = CreateDefaultSubobject<UStaticMeshComponent>("ConvoyerBelt");
	if (!ensure(ConvoyerBelt != nullptr))
		return;
	ConvoyerBelt->SetupAttachment(RootComponent);

	ConvoyerCollider = CreateDefaultSubobject<UBoxComponent>("ConvoyerCollider");
	if (!ensure(ConvoyerCollider != nullptr))
		return;
	ConvoyerCollider->SetupAttachment(ConvoyerBelt);

	ConvoyerDir = CreateDefaultSubobject<UArrowComponent>("ConvoyerDir");
	if (!ensure(ConvoyerDir != nullptr))
		return;
	ConvoyerDir->SetupAttachment(ConvoyerBelt);

	PiecesSpawnPoint = CreateDefaultSubobject<USceneComponent>("PiecesSpawnPoint");
	if (!ensure(PiecesSpawnPoint != nullptr))
		return;
	PiecesSpawnPoint->SetupAttachment(ConvoyerBelt);
}

void AAFA_Conveyor::MoveObjects(float DeltaTime) {
	TArray<AActor*> OverlappingActors;
	ConvoyerCollider->GetOverlappingActors(OverlappingActors);
	for (AActor* Piece : OverlappingActors)
	{
		if (Cast<AAFA_ToyPiece>(Piece)) {
			Piece->AddActorWorldOffset((DeltaTime * Speed) * ConvoyerDir->GetForwardVector());
		}
	}
}

void AAFA_Conveyor::SpawnPieces()
{
	int32 RandIndex = FMath::RandRange(0, PiecesAvailable.Num()-1);
	TSubclassOf<AAFA_ToyPiece> RandomPieceClass = PiecesAvailable[RandIndex];
	
	AAFA_ToyPiece* SpawnedPiece = GetWorld()->SpawnActor<AAFA_ToyPiece>(RandomPieceClass, PiecesSpawnPoint->GetComponentLocation(), FRotator::ZeroRotator);
	if (!ensure(SpawnedPiece != nullptr))
		return;
}

void AAFA_Conveyor::BeginPlay()
{
	Super::BeginPlay();
	SpawnDelay = FMath::RandRange(MinSpawnDelay, MaxSpawnDelay);
}

void AAFA_Conveyor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveObjects(DeltaTime);

	if (SpawnDelay <= 0) {
		SpawnDelay = FMath::RandRange(MinSpawnDelay, MaxSpawnDelay);
		SpawnPieces();
	}
	SpawnDelay -= DeltaTime;
}
