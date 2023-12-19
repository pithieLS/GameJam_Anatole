// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AFA_Conveyor.generated.h"

class AAFA_ToyPiece;

UCLASS()
class GAMEJAM_ANATOLE_API AAFA_Conveyor : public AActor
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAFA_ToyPiece> ToyPieceClass;

	AAFA_Conveyor();

	UFUNCTION(BlueprintCallable)
	void MoveObjects(float DeltaTime);
	void SpawnPieces();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
protected:
	
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* ConvoyerSceneComp;
	
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ConvoyerBelt;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* ConvoyerCollider;

	UPROPERTY(EditDefaultsOnly)
	class UArrowComponent* ConvoyerDir;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AAFA_ToyPiece>> PiecesAvailable;

	UPROPERTY(EditAnywhere)
	class USceneComponent* PiecesSpawnPoint;

	UPROPERTY(EditDefaultsOnly)
	float Speed;
	
	UPROPERTY(EditDefaultsOnly)
	float MinSpawnDelay;
	UPROPERTY(EditDefaultsOnly)
	float MaxSpawnDelay;

private:
	float SpawnDelay;
};
