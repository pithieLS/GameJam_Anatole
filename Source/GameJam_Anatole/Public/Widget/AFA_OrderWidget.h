// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AFA_OrderWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMEJAM_ANATOLE_API UAFA_OrderWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	void InitialiseOrderWidget(FText InToyName, UMaterial* InOverlayTargetMaterial);

protected:

	// Components
	UPROPERTY(meta = (BindWidget))
	class UOverlay* Overlay;
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* VerticalBox;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* LifeTimeProgressBar;
	UPROPERTY(meta = (BindWidget))
	class UImage* ToyOverlayImage;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ToyName;
};
