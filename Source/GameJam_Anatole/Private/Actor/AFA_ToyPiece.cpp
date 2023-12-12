// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AFA_ToyPiece.h"
#include <Components/SphereComponent.h>

// Sets default values
AAFA_ToyPiece::AAFA_ToyPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PieceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PieceMesh"));
	if (!ensure(PieceMesh != nullptr))
		return;
	SetRootComponent(PieceMesh);

	CreateAttachPoints();
}

// Called when the game starts or when spawned
void AAFA_ToyPiece::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAFA_ToyPiece::CreateAttachPoints()
{
	for (USphereComponent* SphereComponent : AttachPoints)
	{
		SphereComponent->DestroyComponent();
	}
	AttachPoints.Empty();

	for (int32 Index = 0; Index < AttachPointsNumber; Index++)
	{
		FString NewAttachPointName = "AttachPoint";
		NewAttachPointName.Append(FString::FromInt(Index));

		USphereComponent* NewAttachPoint = NewObject<USphereComponent>(this);
		if (!ensure(NewAttachPoint != nullptr))
			return;
		NewAttachPoint->RegisterComponent();

		NewAttachPoint->SetupAttachment(RootComponent);

		AttachPoints.Add(NewAttachPoint);
	}
}

#if WITH_EDITOR
void AAFA_ToyPiece::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AAFA_ToyPiece, AttachPointsNumber))
		CreateAttachPoints();
}
#endif

// Called every frame
void AAFA_ToyPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

