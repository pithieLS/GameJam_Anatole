// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFA_LevelActorManager.generated.h"

UCLASS()
class GAMEJAM_ANATOLE_API AAFA_LevelActorManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAFA_LevelActorManager();

	void AddToyPieceToConveyor(int32 ConveyorIndex, TSubclassOf<class AAFA_ToyPiece> ToyPieceToAdd);

	UPROPERTY(EditAnywhere, Category = "LevelActorManager | InLevelActors")
	TArray<class AAFA_PawnMechanicalArm*> PlayersMechanicalArmPawn; // Index 0 == P1, Index 1 == P2, etc...

	UPROPERTY(EditAnywhere, Category = "LevelActorManager | InLevelActors")
	TArray<class AAFA_ToyPieceConveyor*> PlayersToyPieceConveyor; // Index 0 == P1, Index 1 == P2, etc...

	//UPROPERTY(EditAnywhere, Category = "LevelActorManager | InLevelActors")
	//class AAFA_ValidationConveyor* ValidationConveyor;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
