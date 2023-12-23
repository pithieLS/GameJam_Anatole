// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Logic/AFA_ToyVerifier.h"
#include "AFA_ValidationConveyor.generated.h"

class UBoxComponent;
class UWidgetComponent;

UCLASS()
class GAMEJAM_ANATOLE_API AAFA_ValidationConveyor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAFA_ValidationConveyor();

	// Delegates
	DECLARE_MULTICAST_DELEGATE(FOnCurrentVerifierChanged)
	FOnCurrentVerifierChanged OnCurrentVerifierChanged;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void VerifyOverlappedToy(AAFA_ToyPiece* InToyPiece);
	void AddNewVerificationLoop();
	void OnToyVerified(TSubclassOf<UAFA_ToyVerifier>& Verifier, bool bIsValid);
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	void MoveObjects(float DeltaTime);
	void DecrementOrdersLifetime(float DeltaTime);

	// Components
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ConveyorBelt;
	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* ValidationBoxComponent;
	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* BeltCollision;
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* OrderListWidget;

	// Properties
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float BeltSpeed = 100;

	// Verification related
	TMap<TSubclassOf<UAFA_ToyVerifier>, float> CurrentVerifiersToLifeTime;
	float NewVerifierDelay = 10;
	float VerifierLifeTime = 20;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
