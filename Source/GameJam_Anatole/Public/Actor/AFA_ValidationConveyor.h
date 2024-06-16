// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Logic/AFA_ToyOrder.h"
#include "AFA_ConveyorBelt.h"
#include "AFA_ValidationConveyor.generated.h"

class UBoxComponent;
class UWidgetComponent;

UCLASS()
class GAMEJAM_ANATOLE_API AAFA_ValidationConveyor : public AAFA_ConveyorBelt
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAFA_ValidationConveyor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Components
	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* ValidationBoxComponent;

	class AAFA_GameMode* GameMode;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
