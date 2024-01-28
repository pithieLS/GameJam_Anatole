// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Logic/AFA_ToyOrder.h"
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnGameStartedHandler();

	// Verification related
	void VerifyOverlappedToy(AAFA_ToyPiece* InToyPiece);
	void OnToyVerified(UAFA_ToyOrder* VerifiedOrder, bool bIsValid);
	UFUNCTION(BlueprintCallable)
	void MakeNewOrder();
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

	// Properties
	UPROPERTY(EditAnywhere, Category = "Properties")
	float BeltSpeed = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties | Order/Verification")
	TArray<TSubclassOf<UAFA_ToyOrder>> AvailableOrders;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties | Order/Verification")
	float NewOrderDelay = 45;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties | Order/Verification")
	int32 MaxOderNumber = 5;

	class AAFA_GameMode* GameMode;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
