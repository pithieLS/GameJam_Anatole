// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/AFA_OrdersListWidget.h"
#include "Widget/AFA_OrderWidget.h"
#include "Core/AFA_GameMode.h"
#include "Logic/AFA_ToyOrder.h"
#include "Components/HorizontalBox.h"
#include <Kismet/GameplayStatics.h>

void UAFA_OrdersListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AAFA_GameMode* GameMode = Cast<AAFA_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!ensure(GameMode != nullptr))
		return;

	GameMode->OnOrderAddedDelegate.AddUObject(this, &UAFA_OrdersListWidget::AddOrder);
}

void UAFA_OrdersListWidget::AddOrder(UAFA_ToyOrder* NewOrder)
{
	UAFA_OrderWidget* NewOrderWidget = CreateWidget<UAFA_OrderWidget>(this, OrderWidgetClass);

	NewOrderWidget->InitialiseOrderWidget(NewOrder);

	OrderWidgets.Add(NewOrderWidget);
	HorizontalBox->AddChild(NewOrderWidget);
}

