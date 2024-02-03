// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AFA_ConveyorBelt.h"
#include "Components/BoxComponent.h"
#include "../../Public/Actor/AFA_ToyPiece.h"

// Sets default values
AAFA_ConveyorBelt::AAFA_ConveyorBelt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	if (!ensure(SceneComponent != nullptr))
		return;
	SetRootComponent(SceneComponent);

	ConveyorBelt = CreateDefaultSubobject<UStaticMeshComponent>("ConveyorBeltMesh");
	if (!ensure(ConveyorBelt != nullptr))
		return;
	ConveyorBelt->SetupAttachment(RootComponent);

	BeltCollision = CreateDefaultSubobject<UBoxComponent>("BeltCollision");
	if (!ensure(BeltCollision != nullptr))
		return;
	BeltCollision->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AAFA_ConveyorBelt::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAFA_ConveyorBelt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveObjects(DeltaTime);
}

void AAFA_ConveyorBelt::MoveObjects(float DeltaTime)
{
	TArray<AActor*> OverlappedActors;
	BeltCollision->GetOverlappingActors(OverlappedActors);
	for (AActor* Piece : OverlappedActors)
	{
		if (AAFA_ToyPiece* CastedPiece = Cast<AAFA_ToyPiece>(Piece))
		{
			CastedPiece->GetMasterPiece()->AddActorWorldOffset((DeltaTime * BeltSpeed) * ConveyorBelt->GetForwardVector());
		}
	}
}

