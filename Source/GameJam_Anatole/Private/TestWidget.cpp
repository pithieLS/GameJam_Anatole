// Fill out your copyright notice in the Description page of Project Settings.


#include "TestWidget.h"

void UTestWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MyTextBlock->SetText("sqalu");
	MyTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
