// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/AFA_PawnMechanicalArm.h"
#include <Kismet/GameplayStatics.h>
#include "Core/AFA_PlayerController.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/BoxComponent.h"
#include "Actor/AFA_ToyPiece.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Core/GameModes/AFA_GameMode.h"
#include <Components/DecalComponent.h>

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

	GrabbedToyShadowDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("GrabbedToyShadowDecal"));
	if (!ensure(GrabbedToyShadowDecal != nullptr))
		return;
	GrabbedToyShadowDecal->SetupAttachment(RootComponent);
	GrabbedToyShadowDecal->SetVisibility(false);
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

void AAFA_PawnMechanicalArm::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AAFA_GameMode* GameMode = Cast<AAFA_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!ensure(GameMode != nullptr))
		return;

	// Unbind delegates
	GameMode->OnGameStartedDelegate.RemoveAll(this);

	Super::EndPlay(EndPlayReason);
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
	if (ClawLoc.X <= Max_Back && AxisValue < 0)
	{
		Claw->SetRelativeLocation(FVector(Max_Back, ClawLoc.Y, ClawLoc.Z));
		return;
	}
	else if (ClawLoc.X >= Max_Forward && AxisValue > 0)
	{
		Claw->SetRelativeLocation(FVector(Max_Forward, ClawLoc.Y, ClawLoc.Z));
		return;
	}

	//Check if Claw is colliding with anything
	FVector MoveDirection = GetActorForwardVector();
	if (CheckClawCollision(MoveDirection * -1) && AxisValue < 0)
		return;
	else if (CheckClawCollision(MoveDirection) && AxisValue > 0)
		return;

	Claw->AddRelativeLocation(FVector(Move_Speed * AxisValue, 0, 0) * FApp::GetDeltaTime());
}

void AAFA_PawnMechanicalArm::OnRotateClaw(float AxisValue)
{
	if (AxisValue == 0)
		return;

	if (!bCanMove)
		return;

	float ClawRoll = Claw->GetRelativeRotation().Roll;

	// Cap the roll axis
	if (ClawRoll >= Max_Roll && AxisValue < 0)
	{
		Claw->SetRelativeRotation(FRotator(0, 0, Max_Roll));
		return;
	}
	else if (ClawRoll <= Min_Roll && AxisValue > 0)
	{
		Claw->SetRelativeRotation(FRotator(0, 0, Min_Roll));
		return;
	}

	//Check if Claw is colliding with anything
	FVector MoveDirection = FVector(1, 1, 1); // Check all directions
	if (CheckClawCollision(MoveDirection) && AxisValue < 0)
		return;
	else if (CheckClawCollision(MoveDirection * -1) && AxisValue > 0)
		return;

	Claw->AddRelativeRotation(FRotator(0, 0, (Rotate_Speed * AxisValue) * -1) * FApp::GetDeltaTime());
}

void AAFA_PawnMechanicalArm::OnMoveRight(float AxisValue)
{
	if (AxisValue == 0)
		return;

	if (!bCanMove)
		return;

	const FVector ClawLoc = Claw->GetRelativeLocation();

	// Cap the Y axis
	if (ClawLoc.Y <= Max_Left && AxisValue < 0)
	{
		Claw->SetRelativeLocation(FVector(ClawLoc.X, Max_Left, ClawLoc.Z));
		return;
	}
	else if (ClawLoc.Y >= Max_Right && AxisValue > 0)
	{
		Claw->SetRelativeLocation(FVector(ClawLoc.X, Max_Right, ClawLoc.Z));
		return;
	}

	//Check if Claw is colliding with anything
	FVector MoveDirection = GetActorRightVector();
	if (CheckClawCollision(MoveDirection * -1) && AxisValue < 0)
		return;
	else if (CheckClawCollision(MoveDirection) && AxisValue > 0)
		return;

	Claw->AddRelativeLocation(FVector(0, Move_Speed * AxisValue, 0) * FApp::GetDeltaTime());

}

void AAFA_PawnMechanicalArm::OnMoveUp(float AxisValue)
{
	if (AxisValue == 0)
		return;

	if (!bCanMove)
		return;

	FVector ClawLoc = Claw->GetRelativeLocation();

	// Cap the Z axis
	if (ClawLoc.Z <= Max_Down && AxisValue < 0)
	{
		Claw->SetRelativeLocation(FVector(ClawLoc.X, ClawLoc.Y, Max_Down));
		return;
	}
	else if (ClawLoc.Z >= Max_Up && AxisValue > 0)
	{
		Claw->SetRelativeLocation(FVector(ClawLoc.X, ClawLoc.Y, Max_Up));
		return;
	}

	//Check if Claw is colliding with anything
	FVector MoveDirection = GetActorUpVector();
	if (CheckClawCollision(MoveDirection * -1) && AxisValue < 0)
		return;
	else if (CheckClawCollision(MoveDirection) && AxisValue > 0)
		return;

	Claw->AddRelativeLocation(FVector(0, 0, Move_Speed * AxisValue) * FApp::GetDeltaTime());
}

void AAFA_PawnMechanicalArm::GrabDropObject()
{
	if (!bCanMove)
		return;

											// If a toy piece is grabbed, drop it
	if (GrabbedToyPiece != nullptr)
	{
		GrabbedToyPiece->DetachFromArm();
		GrabbedToyShadowDecal->SetVisibility(false);
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

		GrabbedToyShadowDecal->SetVisibility(true);
	}
}

void AAFA_PawnMechanicalArm::WeldObjects() // TODO: A VERIFIER
{
	if (GrabbedToyPiece == nullptr)
		return;

	if (!bCanMove)
		return;

	TArray<AAFA_ToyPiece*> AttachedPieces = GrabbedToyPiece->GetAllAttachedPieces();
	for (AAFA_ToyPiece* ToyPiece : AttachedPieces)
	{
		// en cours: Faire en sorte d'itérer à travers tout les toy piece et attach points et pas que le premier car sinon ça peut ne pas fonctionner et dans le cas ou c'est compacté, ils GetOverlappedToyPieceAttachedPoint retourne que le premier
		TMap<AAFA_ToyPiece*, USphereComponent*> _OverlappedPiecesAndAttachPoints = ToyPiece->GetOverlappedToyPiecesAttachedPoints(nullptr);

		for (TPair<AAFA_ToyPiece*, USphereComponent*> PieceAndAttachPoint : _OverlappedPiecesAndAttachPoints)
		{
			AAFA_ToyPiece* _OverlappedToyPiece = PieceAndAttachPoint.Key;

			if (_OverlappedToyPiece == nullptr)
				continue;
			if (AttachedPieces.Contains(_OverlappedToyPiece))
				continue;

			if (_OverlappedToyPiece->GetAllAttachedPieces().Num() > 1)
			{
				USphereComponent* AttachPointToAttach = PieceAndAttachPoint.Value;
				if (ToyPiece->AttachPointsToPieceMap.Find(AttachPointToAttach) != nullptr) // Check if the attach point is free
					continue;

				TArray<USphereComponent*> OverlappedAttachedPoints;
				_OverlappedToyPiece->GetOverlappedToyPiecesAttachedPoints(ToyPiece).GenerateValueArray(OverlappedAttachedPoints);

				USphereComponent* TargetAttachPoint = OverlappedAttachedPoints[0];
				if (_OverlappedToyPiece->AttachPointsToPieceMap.Contains(TargetAttachPoint))
					continue;

				_OverlappedToyPiece->AttachGroupToToyPiece(AttachPointToAttach, TargetAttachPoint);
				AttachToClaw(GrabbedToyPiece);
				return;
			}
			else
			{
				_OverlappedToyPiece->AttachToToyPiece(ToyPiece);
				return;
			}
		}
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