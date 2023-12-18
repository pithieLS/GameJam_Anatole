// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/TimelineComponent.h"
#include "AFA_PawnMechanicalArm.generated.h"

#define JOYSTICK_TRESHOLD 0.5f

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

	// Getters
	UPhysicsHandleComponent* GetPhysicHandle() { return PhysicHandle; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool CheckClawCollision(FVector Direction) const;

	// Toy rotation related functions
	void RotateToy(FRotator RotationToAdd);
	int32 FindClosestRotationForAxis(const float AxisRotation); // Get the closest axis value from the possible rotations of the object while grabbed
	UFUNCTION()
	void HandleToyRotation(float TLValue);
	UFUNCTION()
    void OnRotateFinished();

	// Input related
	void OnMoveRight(float AxisValue);
	void OnMoveUp(float AxisValue);
	void OnMoveForward(float AxisValue);
	void OnRotateClaw(float AxisValue);
	void OnRequestRotateToy(float AxisValue);
	void GrabDropObject();
	void WeldObjects();
	void UnWeldObjects();

	// Const variables TEMP
	UPROPERTY(EditDefaultsOnly, Category = "Control properties | MoveRange")
	int32 MAX_UP = 100;
	UPROPERTY(EditDefaultsOnly, Category = "Control properties | MoveRange")
	int32 MAX_DOWN = -100;
	UPROPERTY(EditDefaultsOnly, Category = "Control properties | MoveRange")
	int32 MAX_LEFT = -100;
	UPROPERTY(EditDefaultsOnly, Category = "Control properties | MoveRange")
	int32 MAX_RIGHT = 100;
	UPROPERTY(EditDefaultsOnly, Category = "Control properties | MoveRange")
	int32 MAX_FORWARD = 100;
	UPROPERTY(EditDefaultsOnly, Category = "Control properties | MoveRange")
	int32 MAX_BACK = -100;
	UPROPERTY(EditDefaultsOnly, Category = "Control properties | MoveRange")
	int32 MAX_ROLL = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Control properties | MoveRange")
	int32 MIN_ROLL = -150;
	UPROPERTY(EditDefaultsOnly, Category = "Control properties | Control speeds")
	float MOVE_SPEED = 150;
	UPROPERTY(EditDefaultsOnly, Category = "Control properties | Control speeds")
	float ROTATE_SPEED = 100;

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
	FRotator ToyNextRot = FRotator(0, 0, 0);
	FRotator ToyStartRot;
	int32 RotAmount = 90;
	TArray<int32> PossibleRotAngles{-180, -90, 0, 90, 180};

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
