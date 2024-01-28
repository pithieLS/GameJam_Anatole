// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/AFA_OrderWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Logic/AFA_ToyOrder.h"

void UAFA_OrderWidget::InitialiseOrderWidget(UAFA_ToyOrder* InOrder)
{
	AssociatedOrder = InOrder;

	ToyName->SetText(AssociatedOrder->ToyName);
	ToyOverlayImage->SetBrushFromMaterial(AssociatedOrder->OverlayMaterial);

	AssociatedOrder->OnOrderRemovedDelegate.BindUObject(this, &UAFA_OrderWidget::OnOrderRemoved);
}

void UAFA_OrderWidget::OnOrderRemoved()
{
	AssociatedOrder->OnOrderRemovedDelegate.Unbind();

	RemoveFromParent();
}
