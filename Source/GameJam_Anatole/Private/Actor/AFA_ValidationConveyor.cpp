// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AFA_ValidationConveyor.h"
#include "Actor/AFA_ToyPiece.h"
#include "Core/AFA_GameMode.h"
#include "Components/BoxComponent.h"
#include <Kismet/GameplayStatics.h>
#include "Components/WidgetComponent.h"
#include "Actor/AFA_ToyOverlayActor.h"


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

	// Spawns all overlays actors for all available orders
	FVector ToyOverlaySpawnLocation = FVector(0, 0, -500);
	for (TSubclassOf<UAFA_ToyOrder> ToyOrder : AvailableOrders)
	{
		UAFA_ToyOrder* ToyOrderCDO = Cast<UAFA_ToyOrder>(ToyOrder->GetDefaultObject());
		if (!ensure(ToyOrderCDO != nullptr))
			return;

		AAFA_ToyOverlayActor* SpawnedOverlayActor = GetWorld()->SpawnActor<AAFA_ToyOverlayActor>(ToyOrderCDO->ToyOverlayActor, ToyOverlaySpawnLocation, FRotator::ZeroRotator);
		if (!ensure(SpawnedOverlayActor != nullptr))
			return;

		ToyOverlaySpawnLocation.Z -= 150;
	}

	// Bind delegates
	GameMode->OnGameStartedDelegate.AddUObject(this, &AAFA_ValidationConveyor::OnGameStartedHandler);
	ValidationBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AAFA_ValidationConveyor::OnBoxBeginOverlap);
}

void AAFA_ValidationConveyor::VerifyOverlappedToy(AAFA_ToyPiece* InToyPiece)
{
	if (!GameMode->GetIsGameRunning())
		return;

	bool bIsAnyToyValid = false;

	UAFA_ToyOrder* VerifiedOrder = nullptr;
	TArray<UAFA_ToyOrder*> CurrentOrders = GameMode->GetCurrentOrders();
	for (UAFA_ToyOrder* _Order : CurrentOrders)
	{
		if (_Order->VerifyToy(InToyPiece))
		{
			// TODO: make the order with the less lifetime left be the verified order
			bIsAnyToyValid = true;
			VerifiedOrder = _Order;
			break;
		}
	}

	InToyPiece->DestroyToyGroup();
	OnToyVerified(VerifiedOrder, bIsAnyToyValid);
}

void AAFA_ValidationConveyor::OnToyVerified(UAFA_ToyOrder* VerifiedOrder, bool bIsValid)
{
	const int32 ScoreToAdd = bIsValid ? 1 : -1;
	GameMode->AddToScore(ScoreToAdd);

	GameMode->OnToyVerifiedDelegate.Broadcast(bIsValid);

	if(bIsValid)
		GameMode->RemoveOrder(VerifiedOrder);
}

void AAFA_ValidationConveyor::MakeNewOrder()
{
	const int32 RandIndex = FMath::RandRange(0, AvailableOrders.Num() - 1);

	GameMode->AddNewOrder(AvailableOrders[RandIndex]);
}

void AAFA_ValidationConveyor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AAFA_ToyPiece* CastedPiece = Cast<AAFA_ToyPiece>(OtherActor))
		VerifyOverlappedToy(CastedPiece);
}

void AAFA_ValidationConveyor::DecrementOrdersLifetime(float DeltaTime)
{
	for (UAFA_ToyOrder* _Order : GameMode->GetCurrentOrders())
		_Order->DecrementLifetime(DeltaTime);
}

// Called every frame
void AAFA_ValidationConveyor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DecrementOrdersLifetime(DeltaTime);
}

