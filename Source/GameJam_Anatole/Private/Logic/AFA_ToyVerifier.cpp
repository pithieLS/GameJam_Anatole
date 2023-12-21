// Fill out your copyright notice in the Description page of Project Settings.


#include "Logic/AFA_ToyVerifier.h"
#include "Actor/AFA_ToyPiece.h"
#include "Components/SphereComponent.h"

bool UAFA_ToyVerifier::VerifyToy(AAFA_ToyPiece* OverlappedToyPiece)
{
	bool bIsAnyValidPiece = false;
	TArray<AAFA_ToyPiece*> PieceGroup = OverlappedToyPiece->GetAllAttachedPieces();
	for (FPieceVerification CurrentVerif : PiecesVerifications)
	{
		for (AAFA_ToyPiece* ToyPiece : PieceGroup)
		{
			if (!ToyPiece->IsA(CurrentVerif.VerificatedToyPiece))
				continue;

			TMap<USphereComponent*, AAFA_ToyPiece*> AttachPointsToPiece = ToyPiece->AttachPointsToPieceMap;
			for (TPair<FName, TSubclassOf<AAFA_ToyPiece>> AttachedPieceToCheck : CurrentVerif.AttachedToyPieces)
			{
				bIsAnyValidPiece = true;
				const USphereComponent* CheckedAttachPoint = Cast<USphereComponent>(ToyPiece->GetDefaultSubobjectByName(AttachedPieceToCheck.Key));
				AAFA_ToyPiece** CheckedToyPiecePtr = AttachPointsToPiece.Find(CheckedAttachPoint);
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