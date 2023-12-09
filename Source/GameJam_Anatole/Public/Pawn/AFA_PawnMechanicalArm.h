// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/TimelineComponent.h"
#include "AFA_PawnMechanicalArm.generated.h"

class UPhysicsHandleComponent;
class UBoxComponent;
class AAFA_ToyPiece;
class UPhysicsConstraintComponent;

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

	bool CheckClawCollision(FVector Direction);

	// Toy rotation related functions
	void RotateToy(FRotator RotationToAdd);
	UFUNCTION()
	void HandleToyRotation(float TLValue);
	UFUNCTION()
    void OnTimelineFinished();

	// Input related
	void OnMoveRight(float AxisValue);
	void OnMoveUp(float AxisValue);
	void OnMoveForward(float AxisValue);
	void OnRotateClaw(float AxisValue);
	void GrabDropObject();
	void OnRequestRotateToyPitch(float AxisValue);
	void OnRequestRotateToyRoll(float AxisValue);

	// Const variables TEMP
	const int32 MAX_UP = 100;
	const int32 MAX_DOWN = -100;
	const int32 MAX_LEFT = -100;
	const int32 MAX_RIGHT = 100;
	const int32 MAX_FORWARD = 100;
	const int32 MAX_BACK = -100;
	const int32 MAX_ROLL = 0;
	const int32 MIN_ROLL = -150;
	const float MOVE_SPEED = 2; // The claw moving speed scale
	const float ROTATE_SPEED = 2; // The claw rotating speed scale
	const float JOYSTICK_TRESHOLD = 0.5f;

	// Components
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ArmBackMesh;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ArmFrontMesh;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ArmBaseMesh;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ClawMesh;
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* GrabPoint;
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* Claw;	
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* Arm;	
	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* GrabZone;
	UPROPERTY(EditDefaultsOnly)
	UPhysicsHandleComponent* PhysicHandle;
	UPROPERTY(EditDefaultsOnly)
	UPhysicsConstraintComponent* BasePivot;
	UPROPERTY(EditDefaultsOnly)
	UPhysicsConstraintComponent* ArmsPivot;
	UPROPERTY(EditDefaultsOnly)
	UPhysicsConstraintComponent* ClawPivot;

	// Toy related variables
	AAFA_ToyPiece* GrabbedToyPiece = nullptr;
	bool bIsToyRotating = false;
	FTimeline TimelineRot;
	UPROPERTY(EditAnywhere)
	UCurveFloat* CurveFloat;
	FRotator ToyNextRot;
	FRotator ToyStartRot;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
