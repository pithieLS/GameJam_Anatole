// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/AFA_LevelActorManager.h"
#include "Core/AFA_GameInstance.h"
#include "Pawn/AFA_PawnMechanicalArm.h"
#include "Actor/AFA_ToyPieceConveyor.h"

// Sets default values
AAFA_LevelActorManager::AAFA_LevelActorManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AAFA_LevelActorManager::AddToyPieceToConveyor(int32 ConveyorIndex, TSubclassOf<class AAFA_ToyPiece> ToyPieceToAdd)
{
	UE_LOG(LogTemp, Warning, TEXT("%d"), ConveyorIndex);
	PlayersToyPieceConveyor[ConveyorIndex]->AddToyPieceToSpawn(ToyPieceToAdd);
}

// Called when the game starts or when spawned
void AAFA_LevelActorManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAFA_LevelActorManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

