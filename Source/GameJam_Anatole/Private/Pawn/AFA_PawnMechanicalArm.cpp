// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/AFA_PawnMechanicalArm.h"
#include <Kismet/GameplayStatics.h>
#include "Core/AFA_PlayerController.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/BoxComponent.h"
#include "Actor/AFA_ToyPiece.h"

AAFA_PawnMechanicalArm::AAFA_PawnMechanicalArm()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	if (!ensure(SceneComponent != nullptr))
		return;
	SetRootComponent(SceneComponent);

	Claw = CreateDefaultSubobject<USceneComponent>(TEXT("Claw"));
	if (!ensure(Claw != nullptr))
		return;
	Claw->SetupAttachment(RootComponent);

	ClawMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClawMesh"));
	if (!ensure(ClawMesh != nullptr))
		return;
	ClawMesh->SetupAttachment(Claw);

	GrabLocation = CreateDefaultSubobject<USceneComponent>(TEXT("GrabLocation"));
	if (!ensure(GrabLocation != nullptr))
		return;
	GrabLocation->SetupAttachment(Claw);

	GrabZone = CreateDefaultSubobject<UBoxComponent>(TEXT("GrabZone"));
	if(!ensure(GrabZone != nullptr))
		return;
	GrabZone->SetupAttachment(Claw);
	GrabZone->SetHiddenInGame(false);

	PhysicHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	if (!ensure(PhysicHandle != nullptr))
		return;
	PhysicHandle->InterpolationSpeed = 25;
}

// Called when the game starts or when spawned
void AAFA_PawnMechanicalArm::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAFA_PawnMechanicalArm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PhysicHandle->SetTargetLocation(GrabLocation->GetComponentLocation());
}

void AAFA_PawnMechanicalArm::OnMoveForward(float AxisValue)
{
	FVector ClawLoc = Claw->GetRelativeLocation();

	// Cap the X axis
	if (ClawLoc.X <= MAX_BACK && AxisValue < 0)
	{
		Claw->SetRelativeLocation(FVector(MAX_BACK, ClawLoc.Y, ClawLoc.Z));
		return;
	}
	else if (ClawLoc.X >= MAX_FORWARD && AxisValue > 0)
	{
		Claw->SetRelativeLocation(FVector(MAX_FORWARD, ClawLoc.Y, ClawLoc.Z));
		return;
	}

	Claw->AddRelativeLocation(FVector(MOVE_SPEED * AxisValue, 0, 0));
}

void AAFA_PawnMechanicalArm::OnMoveRight(float AxisValue)
{
	FVector ClawLoc = Claw->GetRelativeLocation();

	// Cap the Y axis
	if (ClawLoc.Y <= MAX_LEFT && AxisValue < 0)
	{
		Claw->SetRelativeLocation(FVector(ClawLoc.X, MAX_LEFT, ClawLoc.Z));
		return;
	}
	else if (ClawLoc.Y >= MAX_RIGHT && AxisValue > 0)
	{
		Claw->SetRelativeLocation(FVector(ClawLoc.X, MAX_RIGHT, ClawLoc.Z));
		return;
	}

	Claw->AddRelativeLocation(FVector(0, MOVE_SPEED * AxisValue, 0));

}

void AAFA_PawnMechanicalArm::OnMoveUp(float AxisValue)
{
	FVector ClawLoc = Claw->GetRelativeLocation();

	// Cap the Z axis
	if (ClawLoc.Z <= MAX_DOWN && AxisValue < 0)
	{
		Claw->SetRelativeLocation(FVector(ClawLoc.X, ClawLoc.Y, MAX_DOWN));
		return;
	}
	else if (ClawLoc.Z >= MAX_UP && AxisValue > 0)
	{
		Claw->SetRelativeLocation(FVector(ClawLoc.X, ClawLoc.Y, MAX_UP));
		return;
	}

	Claw->AddRelativeLocation(FVector(0, 0, MOVE_SPEED * AxisValue));
}

void AAFA_PawnMechanicalArm::GrabDropObject()
{
	if (GrabbedToyPiece != nullptr)
	{
		PhysicHandle->ReleaseComponent();
		GrabbedToyPiece = nullptr;
		return;
	}

	// Get all overlapping actors
	TArray<AActor*> OverlappingActors;
	GrabZone->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		AAFA_ToyPiece* CastedActor = Cast<AAFA_ToyPiece>(Actor);
		if (CastedActor != nullptr)
		{
			GrabbedToyPiece = CastedActor;
			break;
		}
	}

	if (GrabbedToyPiece != nullptr)
	{
		// The PhysicHandle needs to have a primitive component so cast the root component
		UPrimitiveComponent* RootPrimitiveComponent = Cast<UPrimitiveComponent>(GrabbedToyPiece->GetRootComponent());

		PhysicHandle->GrabComponent(RootPrimitiveComponent, NAME_None, GrabbedToyPiece->GetActorLocation(), true);
	}
}

// Called to bind functionality to input
void AAFA_PawnMechanicalArm::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveRight", this, &AAFA_PawnMechanicalArm::OnMoveRight);
	PlayerInputComponent->BindAxis("MoveForward", this, &AAFA_PawnMechanicalArm::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveUp", this, &AAFA_PawnMechanicalArm::OnMoveUp);
	PlayerInputComponent->BindAction("GrabDrop", IE_Pressed, this, &AAFA_PawnMechanicalArm::GrabDropObject);
}

