// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AFA_ValidationConveyor.h"
#include "Core/GameModes/AFA_GameMode.h"
#include "Components/BoxComponent.h"
#include <Kismet/GameplayStatics.h>
#include "Components/WidgetComponent.h"
#include "Actor/AFA_ToyPiece.h"


// Sets default values
AAFA_ValidationConveyor::AAFA_ValidationConveyor()
{	
	ValidationBoxComponent = CreateDefaultSubobject<UBoxComponent>("ValidationBoxComponent");
	if (!ensure(ValidationBoxComponent != nullptr))
		return;
	ValidationBoxComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AAFA_ValidationConveyor::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<AAFA_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!ensure(GameMode != nullptr))
		return;

	// Bind delegates
	ValidationBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AAFA_ValidationConveyor::OnBoxBeginOverlap);
}

void AAFA_ValidationConveyor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ValidationBoxComponent->OnComponentBeginOverlap.RemoveAll(this);

	Super::EndPlay(EndPlayReason);
}

void AAFA_ValidationConveyor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AAFA_ToyPiece* CastedPiece = Cast<AAFA_ToyPiece>(OtherActor))
		GameMode->VerifyOverlappedToy(CastedPiece);
}

// Called every frame
void AAFA_ValidationConveyor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

