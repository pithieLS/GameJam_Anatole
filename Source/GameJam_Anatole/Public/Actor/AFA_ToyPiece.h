// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pawn/AFA_PawnMechanicalArm.h"
#include "AFA_ToyPiece.generated.h"

class USphereComponent;

UCLASS()
class GAMEJAM_ANATOLE_API AAFA_ToyPiece : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAFA_ToyPiece();

	FRotator GetClosestRotation(); // Get closest rotation from the ValidRotation array

	// Getters
	UStaticMeshComponent* GetPieceMesh() { return PieceMesh; }
	TArray<AAFA_ToyPiece*> GetAllAttachedPieces();

	// Attach related func/method public
	void DetachFromArm();
	void DetachFromToyPiece();
	void AttachToToyPiece(AAFA_ToyPiece* ToyPieceToAttachTo);
	void AttachGroupToToyPiece(USphereComponent* AttachPointToAttach, USphereComponent* TargetAttachPoint);
	void SetToyGroupCollisionType(ECollisionChannel ChannelName);
	void SetToyGroupCollisionResponseToChannel(ECollisionChannel ChannelName, ECollisionResponse CollisionResponse);
	AAFA_ToyPiece* GetMasterPiece();
	TPair<AAFA_ToyPiece*, USphereComponent*> GetOverlappedToyPieceAttachedPoint(AAFA_ToyPiece* TargetPiece) const; /* Return Key = Toy Piece, Value = AttachPoint
																												    Put nullptr in param if you want a general search*/
	// Attach related
	TPair<AAFA_ToyPiece*, USphereComponent*> OverlappedToyPieceAndAttachPoint;
	AAFA_PawnMechanicalArm* ArmAttachedTo;
	AAFA_ToyPiece* ParentPiece;
	TMap<USphereComponent*, AAFA_ToyPiece*> AttachPointsToPieceMap;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Get the closest axis value from the possible rotations of the object while grabbed
	int32 FindClosestRotationForAxis(const float AxisRotation);

	// Attach related func/methods protected
	void GetAttachedPieces(TArray<AAFA_ToyPiece*>& OutAttachedPieces) const;

	// Components
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* PieceMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USceneComponent* AttachPointsParent;

	// Properties
	TArray<int32> PossibleRotAngles{ -180, -90, 0, 90, 180 };

	FAttachmentTransformRules AttachTransformRules = FAttachmentTransformRules::KeepWorldTransform;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
