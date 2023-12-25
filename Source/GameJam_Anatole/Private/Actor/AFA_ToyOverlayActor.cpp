// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AFA_ToyOverlayActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AAFA_ToyOverlayActor::AAFA_ToyOverlayActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ToyMesh = CreateDefaultSubobject<UStaticMeshComponent>("ToyMesh");
	if (!ensure(ToyMesh != nullptr))
		return;
	SetRootComponent(ToyMesh);

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCaptureComponent");
	if (!ensure(SceneCaptureComponent != nullptr))
		return;
	SceneCaptureComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AAFA_ToyOverlayActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Make scene capture only render toy mesh and have a transparent background
	SceneCaptureComponent->ShowOnlyActorComponents(this);
}

// Called every frame
void AAFA_ToyOverlayActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Make the toy rotate
	ToyMesh->AddRelativeRotation(FQuat(0, 0.5f, 0, 0));
}

