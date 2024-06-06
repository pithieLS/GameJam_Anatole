// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AFA_ToyOrder.generated.h"

class AAFA_ToyPiece;
class USphereComponent;
class UTexture2D;

USTRUCT()
struct FPieceVerification {
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAFA_ToyPiece> VerificatedToyPiece;

	UPROPERTY(EditDefaultsOnly)
	TMap<FName, TSubclassOf<AAFA_ToyPiece>> AttachedToyPieces; // Name of the attach point, piece class

	UPROPERTY(EditDefaultsOnly)
	bool bIgnoreRotation = false;

	UPROPERTY(EditDefaultsOnly)
	FRotator ToyRelativeRotation;
};

/**
 * 
 */
UCLASS(Blueprintable)
class GAMEJAM_ANATOLE_API UAFA_ToyOrder : public UObject
{

	GENERATED_BODY()

public:

	void InitialiseOrder();
	virtual void BeginDestroy() override;

	virtual bool VerifyToy(AAFA_ToyPiece* ToyPieceToCheck);

	void DecrementLifetime(float DeltaTime);

	// Getters
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentLifeTime() { return CurrentLifeTime; }

	// Delegates
	DECLARE_DELEGATE(FOnOrderRemovedDelegate)
	FOnOrderRemovedDelegate OnOrderRemovedDelegate;

	// Properties
	UPROPERTY(EditDefaultsOnly)
	FText ToyName;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AAFA_ToyRTActor> ToyOverlayActor;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* CompleteToyMesh;

	UPROPERTY(EditDefaultsOnly)
	UMaterial* OverlayMaterial;

	UPROPERTY(EditDefaultsOnly)
	TArray<FPieceVerification> PiecesVerifications;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float OrderLifeTime = 30; // Lifetime in seconds

protected:

	float CurrentLifeTime = 0;
};
