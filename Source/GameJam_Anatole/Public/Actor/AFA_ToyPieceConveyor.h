// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actor/AFA_ConveyorBelt.h"
#include "Actor/AFA_ToyPiece.h"
#include "AFA_ToyPieceConveyor.generated.h"

UCLASS()
class GAMEJAM_ANATOLE_API AAFA_ToyPieceConveyor : public AAFA_ConveyorBelt
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAFA_ToyPieceConveyor();

	void AddToyPieceToSpawn(TSubclassOf<class AAFA_ToyPiece> ToyPieceToAdd);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	TSubclassOf<class AAFA_ToyPiece> GetToyPieceToSpawn();
	UFUNCTION(BlueprintCallable)
	void SpawnToyPiece();
	UFUNCTION(BlueprintImplementableEvent)
	void OnGameStartedHandler();

	// Keep track of how many times each pieces has spawned to control the randomness
	TMap<TSubclassOf<class AAFA_ToyPiece>, int32> SpawnedPiecesCount;

	// Components
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* ToyPieceSpawnPoint;

	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	TArray<TSubclassOf<class AAFA_ToyPiece>> AvailableToyPieces;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	float SpawnRate = 5;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
