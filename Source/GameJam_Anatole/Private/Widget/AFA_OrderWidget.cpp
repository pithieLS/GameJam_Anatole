// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/AFA_OrderWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UAFA_OrderWidget::InitialiseOrderWidget(FText InToyName, UMaterial* InOverlayTargetMaterial)
{
	ToyName->SetText(InToyName);
	ToyOverlayImage->SetBrushFromMaterial(InOverlayTargetMaterial);
}
