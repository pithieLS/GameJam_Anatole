// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AFA_ToyRTActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AAFA_ToyRTActor::AAFA_ToyRTActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCaptureComponent");
	if (!ensure(SceneCaptureComponent != nullptr))
		return;
	SetRootComponent(SceneCaptureComponent);

	ToyMesh = CreateDefaultSubobject<UStaticMeshComponent>("ToyMesh");
	if (!ensure(ToyMesh != nullptr))
		return;
	ToyMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AAFA_ToyRTActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Make scene capture only render toy mesh and have a transparent background
	SceneCaptureComponent->ShowOnlyActorComponents(this);
}

