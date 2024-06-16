// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/AFA_ToyOrder.h"
#include "Actor/AFA_ToyPiece.h"
#include "Components/SphereComponent.h"
#include "Core/GameModes/AFA_GameMode.h"
#include <Kismet/GameplayStatics.h>

void UAFA_ToyOrder::BeginDestroy()
{
	OnOrderRemovedDelegate.ExecuteIfBound();

	Super::BeginDestroy();
}

void UAFA_ToyOrder::InitialiseOrder()
{
	CurrentLifeTime = OrderLifeTime;
}

bool UAFA_ToyOrder::VerifyToy(AAFA_ToyPiece* ToyPieceToCheck)
{
	bool bIsAnyValidPiece = false;
	const TArray<AAFA_ToyPiece*> PieceGroup = ToyPieceToCheck->GetAllAttachedPieces();
	for (const FPieceVerification& CurrentVerif : PiecesVerifications)
	{
		for (AAFA_ToyPiece* ToyPiece : PieceGroup)
		{
			if (!ToyPiece->IsA(CurrentVerif.VerificatedToyPiece))
				continue;

			TMap<USphereComponent*, AAFA_ToyPiece*> AttachedPointsToPiece = ToyPiece->AttachPointsToPieceMap;
			for (const TPair<FName, TSubclassOf<AAFA_ToyPiece>>& AttachedPieceToCheck : CurrentVerif.AttachedToyPieces)
			{
				bIsAnyValidPiece = true;
				const USphereComponent* CheckedAttachPoint = Cast<USphereComponent>(ToyPiece->GetDefaultSubobjectByName(AttachedPieceToCheck.Key));
				if (!ensure(CheckedAttachPoint != nullptr))
					continue;

				AAFA_ToyPiece** CheckedToyPiecePtr = AttachedPointsToPiece.Find(CheckedAttachPoint);
				const AAFA_ToyPiece* CheckedToyPiece = CheckedToyPiecePtr ? *CheckedToyPiecePtr : nullptr;

				if (!CheckedToyPiece || !CheckedToyPiece->IsA(AttachedPieceToCheck.Value))
					return false;
			}

		}

		if (!bIsAnyValidPiece)
			return false;
	}

	return true;
}

void UAFA_ToyOrder::DecrementLifetime(float DeltaTime)
{
	CurrentLifeTime -= DeltaTime;

	if (CurrentLifeTime > 0)
		return;

	AAFA_GameMode* GameMode = Cast<AAFA_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!ensure(GameMode != nullptr))
		return;

	GameMode->RemoveOrder(this);
}

//void UAFA_ToyOrder::Tick(float DeltaTime)
//{
//	UE_LOG(LogTemp, Warning, TEXT("Custom tick!"));
//}
