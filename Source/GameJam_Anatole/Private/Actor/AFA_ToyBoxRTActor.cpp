// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AFA_ToyBoxRTActor.h"
#include "Components\SceneCaptureComponent2D.h"
#include "Components\StaticMeshComponent.h"
#include "Logic/AFA_ToyOrder.h"

// Sets default values
AAFA_ToyBoxRTActor::AAFA_ToyBoxRTActor()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>("ScenecComponent");
	if (!ensure(SceneComponent != nullptr))
		return;
	SetRootComponent(SceneComponent);

	SpawnPoint = CreateDefaultSubobject<USceneComponent>("SpawnPoint");
	if (!ensure(SpawnPoint != nullptr))
		return;

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCaptureComponent");
	if (!ensure(SceneCaptureComponent != nullptr))
		return;
	SceneCaptureComponent->SetupAttachment(RootComponent);

	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>("BoxMesh");
	if (!ensure(BoxMesh != nullptr))
		return;
	BoxMesh->SetupAttachment(RootComponent);

}

void AAFA_ToyBoxRTActor::SpawnToyMesh(TSubclassOf<class UAFA_ToyOrder> InOrder)
{
	UStaticMeshComponent* NewToyMesh = NewObject<UStaticMeshComponent>(this);
	if (!ensure(NewToyMesh != nullptr))
		return;

	UAFA_ToyOrder* OrderCDO = Cast<UAFA_ToyOrder>(InOrder->GetDefaultObject());
	if (!ensure(OrderCDO != nullptr))
		return;

	NewToyMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	NewToyMesh->RegisterComponent();
	NewToyMesh->SetWorldScale3D(FVector(0.4f, 0.4f, 0.4f));
	NewToyMesh->SetStaticMesh(OrderCDO->CompleteToyMesh);
	NewToyMesh->SetRelativeLocation(SpawnPoint->GetRelativeLocation());
	NewToyMesh->SetSimulatePhysics(true);
}

// Called when the game starts or when spawned
void AAFA_ToyBoxRTActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAFA_ToyBoxRTActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

