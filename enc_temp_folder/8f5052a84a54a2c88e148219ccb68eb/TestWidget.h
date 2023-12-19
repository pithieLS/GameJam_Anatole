// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TestWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMEJAM_ANATOLE_API UTestWidget : public UUserWidget
{
	GENERATED_BODY()


	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MyTextBlock;

};
