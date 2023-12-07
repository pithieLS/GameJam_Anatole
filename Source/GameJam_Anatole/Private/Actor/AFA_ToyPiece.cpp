// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AFA_ToyPiece.h"

// Sets default values
AAFA_ToyPiece::AAFA_ToyPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PieceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PieceMesh"));
	if (!ensure(PieceMesh != nullptr))
		return;
	SetRootComponent(PieceMesh);
}

// Called when the game starts or when spawned
void AAFA_ToyPiece::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAFA_ToyPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

