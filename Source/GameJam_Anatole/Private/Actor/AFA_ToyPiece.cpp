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

	AttachPointsParent = CreateDefaultSubobject<USceneComponent>(TEXT("AttachPoints"));
	if (!ensure(AttachPointsParent != nullptr))
		return;
	AttachPointsParent->SetupAttachment(RootComponent);
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
}

TPair<AAFA_ToyPiece*, USphereComponent*> AAFA_ToyPiece::GetOverlappedToyPieceAttachedPoint(AAFA_ToyPiece* TargetPiece) const
{
	TArray<USphereComponent*> AttachPointsArray;
	AttachPointsToPieceMap.GenerateKeyArray(AttachPointsArray);

	TArray<AAFA_ToyPiece*> AlreadyAttachedPieces;
	AttachPointsToPieceMap.GenerateValueArray(AlreadyAttachedPieces);

	for (const USphereComponent* _AttachPoint : AttachPointsArray)
	{
		TArray<UPrimitiveComponent*> OverlappingComponents;
		_AttachPoint->GetOverlappingComponents(OverlappingComponents);

		for (UPrimitiveComponent* Component : OverlappingComponents)
			if (USphereComponent* OverlappedAttachPoint = Cast<USphereComponent>(Component))
			{
				AAFA_ToyPiece* CastedToyPiece = Cast<AAFA_ToyPiece>(OverlappedAttachPoint->GetOwner());

				if(TargetPiece != nullptr && CastedToyPiece != TargetPiece)
					continue;
				if(AlreadyAttachedPieces.Contains(CastedToyPiece))
					continue;

				return { CastedToyPiece, OverlappedAttachPoint };
			}
	}

	return {nullptr, nullptr};
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

	ArmAttachedTo->GetPhysicHandle()->ReleaseComponent();
}

void AAFA_ToyPiece::DetachFromToyPiece()
{
	if(ArmAttachedTo)
		DetachFromArm();

	PieceMesh->SetCollisionObjectType(ECC_WorldDynamic);
	PieceMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	PieceMesh->SetSimulatePhysics(true);
	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

	for (TPair<USphereComponent*, AAFA_ToyPiece*> AttachPointPieces : AttachPointsToPieceMap)
	{
		AttachPointsToPieceMap.Add(AttachPointPieces.Key, nullptr);
	}

	ParentPiece = nullptr;
}

void AAFA_ToyPiece::AttachToToyPiece(AAFA_ToyPiece* ToyPieceToAttachTo)
{
	// Prepare the mesh for attachment
	PieceMesh->SetSimulatePhysics(false);
	FAttachmentTransformRules AttachTransformRules = FAttachmentTransformRules::KeepWorldTransform;
	AttachTransformRules.LocationRule = EAttachmentRule::SnapToTarget;
	AttachTransformRules.bWeldSimulatedBodies = true;

	USphereComponent* SelfAttachPoint = ToyPieceToAttachTo->GetOverlappedToyPieceAttachedPoint(nullptr).Value;
	USphereComponent* OtherAttachPoint = GetOverlappedToyPieceAttachedPoint(ToyPieceToAttachTo).Value;

	// Set attached toy piece to both attach points
	AttachPointsToPieceMap.Add(SelfAttachPoint, ToyPieceToAttachTo);
	ToyPieceToAttachTo->AttachPointsToPieceMap.Add(OtherAttachPoint, this);

	// Calculate the attach position and attach to ToyPieceToAttachTo
	const FVector SelfAttachPointLoc = SelfAttachPoint->GetRelativeLocation();
	const FVector OtherAttachPointLoc = OtherAttachPoint->GetRelativeLocation();
	AttachToActor(ToyPieceToAttachTo, AttachTransformRules);
	PieceMesh->SetRelativeLocation(OtherAttachPoint->GetRelativeLocation() * 2);

	SetToyGroupCollisionType(ECC_GameTraceChannel1);
	SetToyGroupCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

	ParentPiece = ToyPieceToAttachTo;
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

