// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AFA_PawnMechanicalArm.generated.h"

class UPhysicsHandleComponent;
class UBoxComponent;
class AAFA_ToyPiece;

UCLASS()
class GAMEJAM_ANATOLE_API AAFA_PawnMechanicalArm : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAFA_PawnMechanicalArm();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Input related
	void OnMoveRight(float AxisValue);
	void OnMoveUp(float AxisValue);
	void OnMoveForward(float AxisValue);
	void GrabDropObject();

	// Const variables
	const int32 MAX_UP = 100;
	const int32 MAX_DOWN = -100;
	const int32 MAX_LEFT = -100;
	const int32 MAX_RIGHT = 100;
	const int32 MAX_FORWARD = 100;
	const int32 MAX_BACK = -100;
	const float MOVE_SPEED = 2; // The claw moving speed scale

	// Components
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ArmBackMesh;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ArmFrontMesh;
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* Claw;	
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ClawMesh;
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* GrabLocation;
	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* GrabZone;
	UPROPERTY(EditDefaultsOnly)
	UPhysicsHandleComponent* PhysicHandle;

	AAFA_ToyPiece* GrabbedToyPiece = nullptr;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
