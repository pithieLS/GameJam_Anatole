// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AFA_ToyVerifier.generated.h"

class AAFA_ToyPiece;
class USphereComponent;

USTRUCT()
struct FPieceVerification {
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAFA_ToyPiece> VerificatedToyPiece;

	UPROPERTY(EditDefaultsOnly)
	TMap<FName, TSubclassOf<AAFA_ToyPiece>> AttachedToyPieces;

	UPROPERTY(EditDefaultsOnly)
	bool bIgnoreRotation = false;

	UPROPERTY(EditDefaultsOnly)
	FRotator ToyRelativeRotation;
};

/**
 * 
 */
UCLASS(Blueprintable)
class GAMEJAM_ANATOLE_API UAFA_ToyVerifier : public UObject
{
	GENERATED_BODY()
public:
	virtual bool VerifyToy(AAFA_ToyPiece* OverlappedToyPiece);

	UPROPERTY(EditDefaultsOnly)
	TArray<FPieceVerification> PiecesVerifications;
};