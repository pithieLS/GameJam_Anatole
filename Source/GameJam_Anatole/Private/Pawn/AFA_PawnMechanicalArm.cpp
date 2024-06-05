// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/AFA_PawnMechanicalArm.h"
#include <Kismet/GameplayStatics.h>
#include "Core/AFA_PlayerController.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/BoxComponent.h"
#include "Actor/AFA_ToyPiece.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Core/GameModes/AFA_GameMode.h"

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

	GrabPoint = CreateDefaultSubobject<USceneComponent>(TEXT("GrabLocation"));
	if (!ensure(GrabPoint != nullptr))
		return;
	GrabPoint->SetupAttachment(Claw);

	Arm = CreateDefaultSubobject<USceneComponent>(TEXT("Arm"));
	if (!ensure(Arm != nullptr))
		return;
	Arm->SetupAttachment(RootComponent);

	ArmBaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArmBaseMesh"));
	if (!ensure(ArmBaseMesh != nullptr))
		return;
	ArmBaseMesh->SetupAttachment(Arm);

	ArmBackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArmBackMesh"));
	if (!ensure(ArmBackMesh != nullptr))
		return;
	ArmBackMesh->SetupAttachment(Arm);

	ArmFrontMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArmFrontMesh"));
	if (!ensure(ArmFrontMesh != nullptr))
		return;
	ArmFrontMesh->SetupAttachment(Arm);

	GrabZone = CreateDefaultSubobject<UBoxComponent>(TEXT("GrabZone"));
	if(!ensure(GrabZone != nullptr))
		return;
	GrabZone->SetupAttachment(Claw);
	GrabZone->SetHiddenInGame(false);

	PhysicHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	if (!ensure(PhysicHandle != nullptr))
		return;
	PhysicHandle->InterpolationSpeed = 25; // Speed of the grabbed object's lag

	BasePivot = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("BasePivot"));
	if (!ensure(BasePivot != nullptr))
		return;
	BasePivot->SetupAttachment(Arm);

	ArmsPivot = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("ArmsPivot"));
	if (!ensure(ArmsPivot != nullptr))
		return;
	ArmsPivot->SetupAttachment(Arm);

	ClawPivot = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("ClawPivot"));
	if (!ensure(ClawPivot != nullptr))
		return;
	ClawPivot->SetupAttachment(Arm);
}

void AAFA_PawnMechanicalArm::DropToyPiece()
{
	PhysicHandle->ReleaseComponent();
	GrabbedToyPiece->ArmAttachedTo = nullptr;
	GrabbedToyPiece = nullptr;
	GrabPoint->SetWorldRotation(FRotator::ZeroRotator);
}

// Called when the game starts or when spawned
void AAFA_PawnMechanicalArm::BeginPlay()
{
	Super::BeginPlay();

	// Bind the Timeline events to functions
	if (!ensure(CurveFloat != nullptr))
		return;
	FOnTimelineFloat TimelineProgress;
	FOnTimelineEvent OnTimelineFinishedCallback;
	TimelineProgress.BindUFunction(this, "HandleToyRotation");
	TimelineRot.AddInterpFloat(CurveFloat, TimelineProgress);
	OnTimelineFinishedCallback.BindUFunction(this, "OnRotateFinished");
	TimelineRot.SetTimelineFinishedFunc(OnTimelineFinishedCallback);

	AAFA_GameMode* GameMode = Cast<AAFA_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!ensure(GameMode != nullptr))
		return;

	// Bind delegates
	GameMode->OnGameStartedDelegate.AddUObject(this, &AAFA_PawnMechanicalArm::OnGameStartedHandler);
}

void AAFA_PawnMechanicalArm::OnGameStartedHandler()
{
	bCanMove = true;
}

bool AAFA_PawnMechanicalArm::CheckClawCollision(FVector Direction) const
{
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // Ignore self

	TArray<FHitResult> HitResults;
	const bool bIsColliding = GetWorld()->SweepMultiByObjectType(
		HitResults,
		ClawMesh->GetComponentLocation(),
		ClawMesh->GetComponentLocation() + Direction * 3, // Use the direction for the collision check and *3 so it never overlap
		FQuat::Identity,
		FCollisionObjectQueryParams::AllStaticObjects, // Only check for world static so it don't check toy parts
		FCollisionShape::MakeBox(ClawMesh->Bounds.BoxExtent),
		CollisionParams
	);

	return bIsColliding;
}

void AAFA_PawnMechanicalArm::AttachToClaw(AAFA_ToyPiece* ToyPieceToAttach)
{
	// The PhysicHandle needs to have a primitive component so cast the root component
	UPrimitiveComponent* RootPrimitiveComponent = Cast<UPrimitiveComponent>(ToyPieceToAttach->GetRootComponent());
	PhysicHandle->GrabComponentAtLocationWithRotation(RootPrimitiveComponent, NAME_None, ToyPieceToAttach->GetActorLocation(), GrabPoint->GetComponentRotation());
}

void AAFA_PawnMechanicalArm::RotateToy(FRotator RotationToAdd)
{
	ToyStartRot = GrabPoint->GetComponentRotation();

	// Avoid Gimbal lock
	FQuat CurrentQuat = FQuat(ToyStartRot);
	FQuat AdditionalQuat = FQuat(RotationToAdd);
	FQuat NewQuat = AdditionalQuat * CurrentQuat;
	ToyNextRot = NewQuat.Rotator();
	
	TimelineRot.PlayFromStart();
}

void AAFA_PawnMechanicalArm::HandleToyRotation(float TLValue)
{
	if (GrabbedToyPiece == nullptr)
	{
		OnRotateFinished();
		return;
	}

	const FQuat NewQuat = FQuat::Slerp(FQuat(ToyStartRot), FQuat(ToyNextRot), TLValue);
	const FRotator NewRotation = NewQuat.Rotator();

	GrabPoint->SetWorldRotation(NewRotation);
}

void AAFA_PawnMechanicalArm::OnRotateFinished()
{
	TimelineRot.Stop();
	bIsToyRotating = false;
}

// Called every frame
void AAFA_PawnMechanicalArm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update the physic handle loc and rot every frame so the toy follows it
	PhysicHandle->SetTargetLocationAndRotation(GrabPoint->GetComponentLocation(), GrabPoint->GetComponentRotation());

	// Set the timeline tick to the delta time
	TimelineRot.TickTimeline(DeltaTime);
}

void AAFA_PawnMechanicalArm::OnMoveForward(float AxisValue)
{
	if (AxisValue == 0)
		return;

	if (!bCanMove)
		return;

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

	//Check if Claw is colliding with anything
	FVector MoveDirection = GetActorForwardVector();
	if (CheckClawCollision(MoveDirection * -1) && AxisValue < 0)
		return;
	else if (CheckClawCollision(MoveDirection) && AxisValue > 0)
		return;

	Claw->AddRelativeLocation(FVector(MOVE_SPEED * AxisValue, 0, 0) * FApp::GetDeltaTime());
}

void AAFA_PawnMechanicalArm::OnRotateClaw(float AxisValue)
{
	if (AxisValue == 0)
		return;

	if (!bCanMove)
		return;

	float ClawRoll = Claw->GetRelativeRotation().Roll;

	// Cap the roll axis
	if (ClawRoll >= MAX_ROLL && AxisValue < 0)
	{
		Claw->SetRelativeRotation(FRotator(0, 0, MAX_ROLL));
		return;
	}
	else if (ClawRoll <= MIN_ROLL && AxisValue > 0)
	{
		Claw->SetRelativeRotation(FRotator(0, 0, MIN_ROLL));
		return;
	}

	//Check if Claw is colliding with anything
	FVector MoveDirection = FVector(1, 1, 1); // Check all directions
	if (CheckClawCollision(MoveDirection) && AxisValue < 0)
		return;
	else if (CheckClawCollision(MoveDirection * -1) && AxisValue > 0)
		return;

	Claw->AddRelativeRotation(FRotator(0, 0, (ROTATE_SPEED * AxisValue) * -1) * FApp::GetDeltaTime());
}

void AAFA_PawnMechanicalArm::OnMoveRight(float AxisValue)
{
	if (AxisValue == 0)
		return;

	if (!bCanMove)
		return;

	const FVector ClawLoc = Claw->GetRelativeLocation();

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

	//Check if Claw is colliding with anything
	FVector MoveDirection = GetActorRightVector();
	if (CheckClawCollision(MoveDirection * -1) && AxisValue < 0)
		return;
	else if (CheckClawCollision(MoveDirection) && AxisValue > 0)
		return;

	Claw->AddRelativeLocation(FVector(0, MOVE_SPEED * AxisValue, 0) * FApp::GetDeltaTime());

}

void AAFA_PawnMechanicalArm::OnMoveUp(float AxisValue)
{
	if (AxisValue == 0)
		return;

	if (!bCanMove)
		return;

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

	//Check if Claw is colliding with anything
	FVector MoveDirection = GetActorUpVector();
	if (CheckClawCollision(MoveDirection * -1) && AxisValue < 0)
		return;
	else if (CheckClawCollision(MoveDirection) && AxisValue > 0)
		return;

	Claw->AddRelativeLocation(FVector(0, 0, MOVE_SPEED * AxisValue) * FApp::GetDeltaTime());
}

void AAFA_PawnMechanicalArm::GrabDropObject()
{
	if (!bCanMove)
		return;

											// If a toy piece is grabbed, drop it
	if (GrabbedToyPiece != nullptr)
	{
		GrabbedToyPiece->DetachFromArm();
		return;
	}
											// If no toy piece grabbed, grab it

	// Get all overlapping actors
	TArray<AActor*> OverlappingActors;
	GrabZone->GetOverlappingActors(OverlappingActors);

	// Grab first find toy
	for (AActor* Actor : OverlappingActors)
	{
		if (AAFA_ToyPiece* CastedActor = Cast<AAFA_ToyPiece>(Actor)) // Check if the overlapped actor is a toy
		{
			GrabbedToyPiece = CastedActor;//CastedActor->GetParentPiece();
			GrabbedToyPiece->ArmAttachedTo = this;
			break;
		}
	}

	// If a toy piece was found, set the master piece rotation to a valid rotation and grab it
	if (GrabbedToyPiece != nullptr)
	{
		AAFA_ToyPiece* MasterPiece = GrabbedToyPiece->GetMasterPiece();
		FRotator NewValidRotation = MasterPiece->GetClosestRotation(false);
		MasterPiece->SetActorRotation(NewValidRotation);

		AttachToClaw(GrabbedToyPiece);
	}
}

void AAFA_PawnMechanicalArm::WeldObjects()
{
	if (GrabbedToyPiece == nullptr)
		return;

	if (!bCanMove)
		return;

	TArray<AAFA_ToyPiece*> AttachedPieces = GrabbedToyPiece->GetAllAttachedPieces();
	for (AAFA_ToyPiece* ToyPiece : AttachedPieces)
	{
		TPair<AAFA_ToyPiece*, USphereComponent*> _OverlappedPieceAndAttachPoint = ToyPiece->GetOverlappedToyPieceAttachedPoint(nullptr);
		AAFA_ToyPiece* _OverlappedToyPiece = _OverlappedPieceAndAttachPoint.Key;

		if (_OverlappedToyPiece == nullptr)
			continue;
		if(AttachedPieces.Contains(_OverlappedToyPiece))
			continue;

		if (_OverlappedToyPiece->GetAllAttachedPieces().Num() > 1)
		{
			USphereComponent* AttachPointToAttach = ToyPiece->GetOverlappedToyPieceAttachedPoint(_OverlappedToyPiece).Value;
			USphereComponent* TargetAttachPoint = _OverlappedToyPiece->GetOverlappedToyPieceAttachedPoint(ToyPiece).Value;

			_OverlappedToyPiece->AttachGroupToToyPiece(AttachPointToAttach, TargetAttachPoint);
			AttachToClaw(GrabbedToyPiece);
		}
		else
			_OverlappedToyPiece->AttachToToyPiece(ToyPiece);
		break;
	}
}

void AAFA_PawnMechanicalArm::UnWeldObjects()
{
	if (GrabbedToyPiece == nullptr)
		return;

	if (!bCanMove)
		return;

	TArray<AAFA_ToyPiece*> AttachedPieces = GrabbedToyPiece->GetAllAttachedPieces();
	for (AAFA_ToyPiece* ToyPiece : AttachedPieces)
	{
		ToyPiece->DetachFromToyPiece();
	}

	GrabbedToyPiece = nullptr;
}

void AAFA_PawnMechanicalArm::OnRequestRotateToy(float AxisValue)
{
	if (!bCanMove)
		return;

	// If grabbed object is rotating, return
	if (bIsToyRotating)
		return;

	// If there is no grabbed object, return
	if (GrabbedToyPiece == nullptr)
		return;

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!ensure(PlayerController != nullptr))
		return;

	float RightThumbstickX, RightThumbstickY;
	PlayerController->GetInputAnalogStickState(EControllerAnalogStick::CAS_RightStick, RightThumbstickX, RightThumbstickY);

	if (abs(RightThumbstickX) < JOYSTICK_TRESHOLD && abs(RightThumbstickY) < JOYSTICK_TRESHOLD)
		return;

	// Check X Axis (Pitch)
	int PitchDirection;
	if (RightThumbstickX > JOYSTICK_TRESHOLD) // Joystick up case
		PitchDirection = -1;
	else if (RightThumbstickX < JOYSTICK_TRESHOLD * -1) // Joystick down case
		PitchDirection = 1;
	else
		PitchDirection = 0;

	//Check Y Axis (Roll)
	int RollDirection;
	if (RightThumbstickY > JOYSTICK_TRESHOLD) // Joystick right case
		RollDirection = 1;
	else if (RightThumbstickY < JOYSTICK_TRESHOLD * -1) // Joystick left case
		RollDirection = -1;
	else
		RollDirection = 0;

	// If the joystick is more on an axis A than the axis B, rotate the toy piece on axis A
	if(abs(RightThumbstickX) > abs(RightThumbstickY))
		RotateToy(FRotator(0, 90 * PitchDirection, 0));
	else if (abs(RightThumbstickY) > abs(RightThumbstickX))
		RotateToy(FRotator(90 * RollDirection, 0, 0));

	bIsToyRotating = true;
}

// Called to bind functionality to input
void AAFA_PawnMechanicalArm::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveRight", this, &AAFA_PawnMechanicalArm::OnMoveRight);
	PlayerInputComponent->BindAxis("MoveForward", this, &AAFA_PawnMechanicalArm::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveUp", this, &AAFA_PawnMechanicalArm::OnMoveUp);
	PlayerInputComponent->BindAxis("Rotate", this, &AAFA_PawnMechanicalArm::OnRotateClaw);
	PlayerInputComponent->BindAxis("RotateToy", this, &AAFA_PawnMechanicalArm::OnRequestRotateToy);
	PlayerInputComponent->BindAction("GrabDrop", IE_Pressed, this, &AAFA_PawnMechanicalArm::GrabDropObject);
	PlayerInputComponent->BindAction("WeldObjects", IE_Pressed, this, &AAFA_PawnMechanicalArm::WeldObjects);
	PlayerInputComponent->BindAction("UnWeldObjects", IE_Pressed, this, &AAFA_PawnMechanicalArm::UnWeldObjects);
}