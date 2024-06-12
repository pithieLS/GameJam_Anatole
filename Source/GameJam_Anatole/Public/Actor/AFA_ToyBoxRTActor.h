// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AFA_ToyBoxRTActor.generated.h"

UCLASS()
class GAMEJAM_ANATOLE_API AAFA_ToyBoxRTActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAFA_ToyBoxRTActor();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SpawnVerifiedToys(const TMap<TSubclassOf<class UAFA_ToyOrder>, int32>& InOrderVerificationCount);
	UFUNCTION(BlueprintCallable)
	void SpawnToyMesh(TSubclassOf<class UAFA_ToyOrder> InOrder);
	UPROPERTY(EditDefaultsOnly)
	int32 BoxRotationSpeed = 45; // Degrees per seconds

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<UStaticMeshComponent*> SpawnedStaticMeshes;

	// Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USceneComponent* SpawnPoint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent* BoxMesh;
	UPROPERTY(EditDefaultsOnly)
	class USceneCaptureComponent2D* SceneCaptureComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
