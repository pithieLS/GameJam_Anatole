// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AFA_ToyPiece.h"
#include <Components/SphereComponent.h>
#include "Pawn/AFA_PawnMechanicalArm.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

// Sets default values
AAFA_ToyPiece::AAFA_ToyPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PieceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PieceMesh"));
	if (!ensure(PieceMesh != nullptr))
		return;
	SetRootComponent(PieceMesh);
	PieceMesh->SetSimulatePhysics(true);

	AttachPointsParent = CreateDefaultSubobject<USceneComponent>(TEXT("AttachPointsParent"));
	if (!ensure(AttachPointsParent != nullptr))
		return;
	AttachPointsParent->SetupAttachment(RootComponent);
}

void AAFA_ToyPiece::DestroyToyGroup()
{
	for (AAFA_ToyPiece* Piece : GetAllAttachedPieces())
		Piece->Destroy();
}

// Called when the game starts or when spawned
void AAFA_ToyPiece::BeginPlay()
{
	Super::BeginPlay();
	
	// Put all attach points in an array ///and bind them to OnBeginOverlap function
	TArray<USceneComponent*> ChildComponents;
	AttachPointsParent->GetChildrenComponents(true, ChildComponents);
	for (USceneComponent* Child : ChildComponents)
		if (USphereComponent* AttachPoint = Cast<USphereComponent>(Child))
			AttachPointsToPieceMap.Add(AttachPoint, nullptr);

	// Initialize attachment rules for welding
	AttachTransformRules.LocationRule = EAttachmentRule::SnapToTarget;
	AttachTransformRules.bWeldSimulatedBodies = true;
}

int32 AAFA_ToyPiece::FindClosestRotationForAxis(const float AxisRotation)
{
	// Initialize variables to keep track of the closest rotation and its angular distance
	int32 ClosestRotation = 0;
	int32 MinAngularDistance = INT32_MAX;

	for (const int32 PossibleRotation : PossibleRotAngles)
	{
		float AngularDistance = FMath::Abs(FRotator::NormalizeAxis(PossibleRotation - AxisRotation));

		// Update the closest rotation if the current rotation is closer
		if (AngularDistance < MinAngularDistance)
		{
			MinAngularDistance = AngularDistance;
			ClosestRotation = PossibleRotation;
		}
	}

	return ClosestRotation;
}

FRotator AAFA_ToyPiece::GetClosestRotation(bool bIsGettingClosestRelativeRot)
{
	FRotator CurrentRotation;
	if (bIsGettingClosestRelativeRot)
		CurrentRotation = RootComponent->GetRelativeRotation();
	else
		CurrentRotation = GetActorRotation();

	FRotator PieceRot;
	PieceRot.Pitch = FindClosestRotationForAxis(CurrentRotation.Pitch);
	PieceRot.Yaw = FindClosestRotationForAxis(CurrentRotation.Yaw);
	PieceRot.Roll = FindClosestRotationForAxis(CurrentRotation.Roll);

	return PieceRot;
}

TMap<AAFA_ToyPiece*, USphereComponent*> AAFA_ToyPiece::GetOverlappedToyPiecesAttachedPoints(AAFA_ToyPiece* TargetPiece) const
{
	TArray<USphereComponent*> AttachPointsArray;
	AttachPointsToPieceMap.GenerateKeyArray(AttachPointsArray);

	TArray<AAFA_ToyPiece*> AlreadyAttachedPieces;
	AttachPointsToPieceMap.GenerateValueArray(AlreadyAttachedPieces);

	TMap<AAFA_ToyPiece*, USphereComponent*> ToyPiecesAttachedPoints;

	for (const USphereComponent* _AttachPoint : AttachPointsArray)
	{
		TArray<UPrimitiveComponent*> OverlappingComponents;
		_AttachPoint->GetOverlappingComponents(OverlappingComponents);

		for (UPrimitiveComponent* Component : OverlappingComponents)
			if (USphereComponent* OverlappedAttachPoint = Cast<USphereComponent>(Component))
			{
				AAFA_ToyPiece* CastedToyPiece = Cast<AAFA_ToyPiece>(OverlappedAttachPoint->GetOwner());

				if(CastedToyPiece == this)
					continue;
				if(TargetPiece != nullptr && CastedToyPiece != TargetPiece)
					continue;
				if(AlreadyAttachedPieces.Contains(CastedToyPiece))
					continue;

				ToyPiecesAttachedPoints.Add(CastedToyPiece, OverlappedAttachPoint);
			}
	}

	return ToyPiecesAttachedPoints;
}

TArray<AAFA_ToyPiece*> AAFA_ToyPiece::GetAllAttachedPieces()
{
	TArray<AAFA_ToyPiece*> AttachedPieces;
	AAFA_ToyPiece* MasterPiece = GetMasterPiece();
	AttachedPieces.Add(MasterPiece);
	MasterPiece->GetAttachedPieces(AttachedPieces);
	return AttachedPieces;
}

void AAFA_ToyPiece::GetAttachedPieces(TArray<AAFA_ToyPiece*>& OutAttachedPieces) const
{
	for (const TPair<USphereComponent*, AAFA_ToyPiece*> AttachPointAndPiece : AttachPointsToPieceMap)
	{
		if(AttachPointAndPiece.Value == nullptr)
			continue;
		if(OutAttachedPieces.Contains(AttachPointAndPiece.Value))
			continue;

		OutAttachedPieces.Add(AttachPointAndPiece.Value);
		AttachPointAndPiece.Value->GetAttachedPieces(OutAttachedPieces);
	}
}

void AAFA_ToyPiece::DetachFromArm()
{
	if (ArmAttachedTo == nullptr)
		return;

	ArmAttachedTo->DropToyPiece();
}

void AAFA_ToyPiece::DetachFromToyPiece()
{
	if(ArmAttachedTo != nullptr)
		DetachFromArm();

	//PieceMesh->SetCollisionObjectType(ECC_WorldDynamic);
	//PieceMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	PieceMesh->SetSimulatePhysics(true);
	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

	TMap<USphereComponent*, AAFA_ToyPiece*> AttachPointToPieceMapCopy = AttachPointsToPieceMap;
	for (TPair<USphereComponent*, AAFA_ToyPiece*> AttachPointPieces : AttachPointToPieceMapCopy)
	{
		AttachPointsToPieceMap.Add(AttachPointPieces.Key, nullptr);
	}

	ParentPiece = nullptr;
}

void AAFA_ToyPiece::AttachToToyPiece(AAFA_ToyPiece* ToyPieceToAttachTo)
{
	AAFA_ToyPiece* MasterPiece = GetMasterPiece();
	if (!ensure(MasterPiece != nullptr))
		return;

	for (AAFA_ToyPiece* _ToyPiece : GetAllAttachedPieces())
		if(_ToyPiece->ArmAttachedTo != nullptr)
			DetachFromArm();

	// Prepare the mesh for attachment
	PieceMesh->SetSimulatePhysics(false);

	// Get attach points of both pieces
	TArray<USphereComponent*> FoundSelfAttachPoints;
	TArray<USphereComponent*> FoundTargetAttachPoints;
	ToyPieceToAttachTo->GetOverlappedToyPiecesAttachedPoints(this).GenerateValueArray(FoundSelfAttachPoints);
	GetOverlappedToyPiecesAttachedPoints(ToyPieceToAttachTo).GenerateValueArray(FoundTargetAttachPoints);

	USphereComponent* SelfAttachPoint = FoundSelfAttachPoints[0];
	USphereComponent* TargetAttachPoint = FoundTargetAttachPoints[0];

	if(FoundSelfAttachPoints.Num() > 1 || FoundTargetAttachPoints.Num() > 1)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("(!) Multiple attach points of non attach piece overlapped (!)"));

	// Set attached toy piece to both attach points
	AttachPointsToPieceMap.Add(SelfAttachPoint, ToyPieceToAttachTo);
	ToyPieceToAttachTo->AttachPointsToPieceMap.Add(TargetAttachPoint, this);

	// Calculate the attach position and attach to ToyPieceToAttachTo
	AttachToActor(ToyPieceToAttachTo, AttachTransformRules);
	FRotator PieceRot = GetClosestRotation(true);
	SetActorRelativeRotation(PieceRot);
	FVector DistanceRootAndAttach = GetActorLocation() - SelfAttachPoint->GetComponentLocation();
	SetActorLocation(TargetAttachPoint->GetChildComponent(0)->GetComponentLocation() + DistanceRootAndAttach);

	//SetToyGroupCollisionType(ECC_GameTraceChannel1);
	//SetToyGroupCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	ParentPiece = ToyPieceToAttachTo;
}

void AAFA_ToyPiece::AttachGroupToToyPiece(USphereComponent* AttachPointToAttach, USphereComponent* TargetAttachPoint)
{
	AAFA_ToyPiece* MasterPiece = GetMasterPiece();
	if (!ensure(MasterPiece != nullptr))
		return;
	AAFA_ToyPiece* TargetPiece = Cast<AAFA_ToyPiece>(TargetAttachPoint->GetOwner());
	if (!ensure(TargetPiece != nullptr))
		return;

	for (AAFA_ToyPiece* _ToyPiece : GetAllAttachedPieces())
		if (_ToyPiece->ArmAttachedTo != nullptr)
			DetachFromArm();

	// Set attached toy piece to both attach points
	AttachPointsToPieceMap.Add(AttachPointToAttach, TargetPiece);
	TargetPiece->AttachPointsToPieceMap.Add(TargetAttachPoint, this);

	// Attach the actor and calculate the attach position and rotation
	MasterPiece->AttachToActor(TargetPiece, AttachTransformRules);
	FRotator PieceRot = MasterPiece->GetClosestRotation(true);
	MasterPiece->SetActorRelativeRotation(PieceRot);
	MasterPiece->PieceMesh->SetSimulatePhysics(false);
	FVector DistanceAttachedPieceAndMasterPiece = MasterPiece->GetActorLocation() - GetActorLocation();
	FVector DistanceRootAndAttach = GetActorLocation() - AttachPointToAttach->GetChildComponent(0)->GetComponentLocation();
	FVector AttachLocation = TargetAttachPoint->GetChildComponent(0)->GetComponentLocation() + DistanceRootAndAttach;
	//FRotator PieceRot = MasterPiece->GetClosestRotation();
	//MasterPiece->SetActorRotation(PieceRot);
	MasterPiece->SetActorLocation(AttachLocation + DistanceAttachedPieceAndMasterPiece);

	// Update collision because a bug cause the collisions to stay in place while the mesh is at another location
	for (AAFA_ToyPiece* _ToyPiece : MasterPiece->GetAllAttachedPieces())
	{
		_ToyPiece->GetPieceMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		_ToyPiece->GetPieceMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	//SetToyGroupCollisionType(ECC_GameTraceChannel1);
	//SetToyGroupCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	MasterPiece->ParentPiece = TargetPiece;

	for (AAFA_ToyPiece* _ToyPiece : MasterPiece->GetAllAttachedPieces())
	{
		FString ActorName = _ToyPiece->GetName();
		UE_LOG(LogTemp, Warning, TEXT("Actor Name: %s"), *ActorName);
	}
}

void AAFA_ToyPiece::SetToyGroupCollisionType(ECollisionChannel ChannelName)
{
	AAFA_ToyPiece* MasterPiece = GetMasterPiece();
	for (AAFA_ToyPiece* _ToyPiece : MasterPiece->GetAllAttachedPieces())
		_ToyPiece->PieceMesh->SetCollisionObjectType(ChannelName);
}

void AAFA_ToyPiece::SetToyGroupCollisionResponseToChannel(ECollisionChannel ChannelName, ECollisionResponse CollisionResponse)
{
	AAFA_ToyPiece* MasterPiece = GetMasterPiece();
	for (AAFA_ToyPiece* _ToyPiece : MasterPiece->GetAllAttachedPieces())
		_ToyPiece->PieceMesh->SetCollisionResponseToChannel(ChannelName, CollisionResponse);
}

AAFA_ToyPiece* AAFA_ToyPiece::GetMasterPiece()
{
	if (ParentPiece == nullptr)
		return this;

	return ParentPiece->GetMasterPiece();
}

// Called every frame
void AAFA_ToyPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Avoid ToyPiece from rotating because of it's angular velocity
	//PieceMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
}

