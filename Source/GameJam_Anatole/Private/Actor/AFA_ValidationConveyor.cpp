// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AFA_ValidationConveyor.h"
#include "Actor/AFA_ToyPiece.h"
#include "Core/AFA_GameMode.h"
#include "Components/BoxComponent.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AAFA_ValidationConveyor::AAFA_ValidationConveyor()
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

	ValidationBoxComponent = CreateDefaultSubobject<UBoxComponent>("ValidationBoxComponent");
	if (!ensure(ValidationBoxComponent != nullptr))
		return;
	ValidationBoxComponent->SetupAttachment(RootComponent);

	BeltCollision = CreateDefaultSubobject<UBoxComponent>("BeltCollision");
	if (!ensure(BeltCollision != nullptr))
		return;
	BeltCollision->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AAFA_ValidationConveyor::BeginPlay()
{
	Super::BeginPlay();
	
	AddNewVerificationLoop();
	ValidationBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AAFA_ValidationConveyor::OnBoxBeginOverlap);
}

void AAFA_ValidationConveyor::VerifyOverlappedToy(AAFA_ToyPiece* InToyPiece)
{
	bool bIsAnyToyValid = false;

	TSubclassOf<UAFA_ToyVerifier> VerifiedVerifier;
	for (TSubclassOf<UAFA_ToyVerifier> _Verifier : CurrentVerifiers)
	{
		UAFA_ToyVerifier* ToyVerifierCDO = Cast<UAFA_ToyVerifier>(_Verifier->GetDefaultObject());
		if (!ensure(ToyVerifierCDO != nullptr))
			return;
		if (ToyVerifierCDO->VerifyToy(InToyPiece))
		{
			bIsAnyToyValid = true;
			VerifiedVerifier = _Verifier;
			InToyPiece->DestroyToyGroup();
			break;
		}
	}

	OnToyVerified(VerifiedVerifier, bIsAnyToyValid);
}

void AAFA_ValidationConveyor::AddNewVerificationLoop()
{
	AAFA_GameMode* GameMode = Cast<AAFA_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!ensure(GameMode != nullptr))
		return;

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this, GameMode]()
		{
			const int32 RandIndex = FMath::RandRange(0, GameMode->AvailableVerifiers.Num() - 1);
			CurrentVerifiers.Add(GameMode->AvailableVerifiers[RandIndex]);
			OnCurrentVerifierChanged.Broadcast();
		}, 3, true, NewVerifierDelay);
}

void AAFA_ValidationConveyor::OnToyVerified(TSubclassOf<UAFA_ToyVerifier>& Verifier, bool bIsValid)
{
	AAFA_GameMode* GameMode = Cast<AAFA_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!ensure(GameMode != nullptr))
		return;

	const int32 ScoreToAdd = bIsValid ? 1 : -1;
	GameMode->AddToScore(ScoreToAdd);
	CurrentVerifiers.Remove(Verifier);
	OnCurrentVerifierChanged.Broadcast();

	UE_LOG(LogTemp, Warning, TEXT("ToyVerfied: %d"), ScoreToAdd);
}

void AAFA_ValidationConveyor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AAFA_ToyPiece* CastedPiece = Cast<AAFA_ToyPiece>(OtherActor))
		VerifyOverlappedToy(CastedPiece);
}

void AAFA_ValidationConveyor::MoveObjects(float DeltaTime) {
	TArray<AActor*> OverlappedActors;
	BeltCollision->GetOverlappingActors(OverlappedActors);
	for (AActor* Piece : OverlappedActors)
	{
		if (AAFA_ToyPiece* CastedPiece = Cast<AAFA_ToyPiece>(Piece))
			if(CastedPiece->GetMasterPiece() == CastedPiece)
				Piece->AddActorWorldOffset((DeltaTime * BeltSpeed) * ConveyorBelt->GetForwardVector());
	}
}

// Called every frame
void AAFA_ValidationConveyor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveObjects(DeltaTime);
}

