// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AFA_OrdersListWidget.generated.h"

/**
 * 
 */

class UAFA_ToyOrder;
class UAFA_OrderWidget;

UCLASS()
class GAMEJAM_ANATOLE_API UAFA_OrdersListWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	void AddOrder(UAFA_ToyOrder* NewOrder);

	TArray<UAFA_OrderWidget*> OrderWidgets;

	// Components
	UPROPERTY(meta = (BindWidget))
	class UOverlay* Overlay;
	UPROPERTY(meta = (BindWidget))
	class UImage* BackgroundImage;
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HorizontalBox;

	// Properties
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAFA_OrderWidget> OrderWidgetClass;
};
