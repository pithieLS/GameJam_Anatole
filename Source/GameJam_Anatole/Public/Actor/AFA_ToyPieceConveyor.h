// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actor/AFA_ConveyorBelt.h"
#include "AFA_ToyPieceConveyor.generated.h"

UCLASS()
class GAMEJAM_ANATOLE_API AAFA_ToyPieceConveyor : public AAFA_ConveyorBelt
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAFA_ToyPieceConveyor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SpawnToyPiece();

	// Components
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* ToyPieceSpawnPoint;

	// Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<TSubclassOf<class AAFA_ToyPiece>> ToyPiecesToSpawn;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
