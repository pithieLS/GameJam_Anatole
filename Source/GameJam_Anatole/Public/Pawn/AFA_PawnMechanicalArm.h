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

	// Getters & Setters
	UPhysicsHandleComponent* GetPhysicHandle() { return PhysicHandle; }
	
	// Grab/Drop related
	void DropToyPiece();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnGameStartedHandler();

	bool CheckClawCollision(FVector Direction) const;
	void AttachToClaw(AAFA_ToyPiece* ToyPieceToAttach);

	// Toy rotation related functions
	void RotateToy(FRotator RotationToAdd);
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
	UPROPERTY(EditAnywhere, Category = "Control properties | MoveRange")
	int32 MAX_UP = 200;
	UPROPERTY(EditAnywhere, Category = "Control properties | MoveRange")
	int32 MAX_DOWN = -100;
	UPROPERTY(EditAnywhere, Category = "Control properties | MoveRange")
	int32 MAX_LEFT = -110;
	UPROPERTY(EditAnywhere, Category = "Control properties | MoveRange")
	int32 MAX_RIGHT = 100;
	UPROPERTY(EditAnywhere, Category = "Control properties | MoveRange")
	int32 MAX_FORWARD = 100;
	UPROPERTY(EditAnywhere, Category = "Control properties | MoveRange")
	int32 MAX_BACK = -100;
	UPROPERTY(EditAnywhere, Category = "Control properties | MoveRange")
	int32 MAX_ROLL = 150;
	UPROPERTY(EditAnywhere, Category = "Control properties | MoveRange")
	int32 MIN_ROLL = -150;
	UPROPERTY(EditAnywhere, Category = "Control properties | Control speeds")
	float MOVE_SPEED = 150;
	UPROPERTY(EditAnywhere, Category = "Control properties | Control speeds")
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* GrabPoint;
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* Claw;	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UDecalComponent* GrabbedToyShadowDecal;

	// Toy related variables
	AAFA_ToyPiece* GrabbedToyPiece = nullptr;
	bool bIsToyRotating = false;
	FTimeline TimelineRot;
	UPROPERTY(EditAnywhere)
	UCurveFloat* CurveFloat;
	FRotator ToyNextRot = FRotator(0, 0, 0);
	FRotator ToyStartRot;
	int32 RotAmount = 90;

	bool bCanMove = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
