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

	GameMode->OnOrdersChangedDelegate.AddUObject(this, &UAFA_OrdersListWidget::OnOrdersChanged);
}

void UAFA_OrdersListWidget::OnOrdersChanged()
{
	AAFA_GameMode* GameMode = Cast<AAFA_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!ensure(GameMode != nullptr))
		return;

	for (UAFA_OrderWidget* OrderUMG : OrderWidgets)
	{
		OrderUMG->RemoveFromParent();
		OrderUMG->ConditionalBeginDestroy();
	}

	OrderWidgets.Empty();

	TArray<UAFA_ToyOrder*> CurrentOrders = GameMode->GetCurrentOrders();
	for (UAFA_ToyOrder* _Order : CurrentOrders)
	{
		AddOrder(_Order);
	}
}

void UAFA_OrdersListWidget::AddOrder(UAFA_ToyOrder* NewOrder)
{
	UAFA_OrderWidget* NewOrderWidget = CreateWidget<UAFA_OrderWidget>(this, OrderWidgetClass);

	NewOrderWidget->InitialiseOrderWidget(NewOrder->ToyName, NewOrder->OverlayMaterial);

	OrderWidgets.Add(NewOrderWidget);
	HorizontalBox->AddChild(NewOrderWidget);
}

