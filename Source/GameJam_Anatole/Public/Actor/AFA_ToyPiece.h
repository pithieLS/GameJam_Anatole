// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFA_ToyPiece.generated.h"

class USphereComponent;

UCLASS()
class GAMEJAM_ANATOLE_API AAFA_ToyPiece : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAFA_ToyPiece();

	// Getters
	UStaticMeshComponent* GetPieceMesh() { return PieceMesh; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void CreateAttachPoints();

	// Called when a property change
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override; // Call Constructor when AttachPointsNumber is changed
#endif

	// Toy piece properties
	UPROPERTY(EditDefaultsOnly, Category = "ToyPiece Properties")
	int32 AttachPointsNumber;

	// Components
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* PieceMesh;
	TArray<USphereComponent*> AttachPoints;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
