// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFA_ToyRTActor.generated.h"

UCLASS()
class GAMEJAM_ANATOLE_API AAFA_ToyRTActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAFA_ToyRTActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void RotateToyOverlay(float DeltaTime); // Make the toy overlay rotate

	// Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent* ToyMesh;
	UPROPERTY(EditDefaultsOnly)
	class USceneCaptureComponent2D* SceneCaptureComponent;
};
