// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AFA_ConveyorBelt.generated.h"

UCLASS()
class GAMEJAM_ANATOLE_API AAFA_ConveyorBelt : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAFA_ConveyorBelt();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveObjects(float DeltaTime);

	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	float BeltSpeed = 100;

	// Components
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* ConveyorBelt;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* BeltCollision;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
