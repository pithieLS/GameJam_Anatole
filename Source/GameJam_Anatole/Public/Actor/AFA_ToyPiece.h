// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFA_ToyPiece.generated.h"

UCLASS()
class GAMEJAM_ANATOLE_API AAFA_ToyPiece : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAFA_ToyPiece();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Components
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* PieceMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
