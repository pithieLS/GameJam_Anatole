// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AFA_CountdownWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMEJAM_ANATOLE_API UAFA_CountdownWidget : public UUserWidget
{
	GENERATED_BODY()

public:

protected:

	// Properties
	UPROPERTY(BlueprintReadWrite)
	TArray<class UTexture2D*> CountdownNumbers;
	UPROPERTY(BlueprintReadWrite)
	class AAFA_GameMode* GameMode;

	// Components
	UPROPERTY(meta = (BindWidget))
	class UOverlay* CountdownOverlay;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UImage* NumbersImage;
};
