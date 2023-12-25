// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/AFA_OrdersListWidget.h"
#include "Widget/AFA_OrderWidget.h"
#include "Core/AFA_GameMode.h"
#include "Logic/AFA_ToyVerifier.h"
#include "Components/HorizontalBox.h"
#include <Kismet/GameplayStatics.h>

void UAFA_OrdersListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AAFA_GameMode* GameMode = Cast<AAFA_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!ensure(GameMode != nullptr))
		return;

	GameMode->OnOrdersChanged.AddUObject(this, &UAFA_OrdersListWidget::OnOrdersChanged);
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

	TMap<TSubclassOf<UAFA_ToyVerifier>, float>& CurrentOrders = GameMode->GetCurrentOrders();
	for (TPair<TSubclassOf<UAFA_ToyVerifier>, float>& _VerifierToLifeTime : CurrentOrders)
	{
		AddOrder(_VerifierToLifeTime.Key); // Only one order because it can't add two times the same thing in a TMap, to fix
	}
}

void UAFA_OrdersListWidget::AddOrder(TSubclassOf<UAFA_ToyVerifier>& NewOrder)
{
	UAFA_OrderWidget* NewOrderWidget = CreateWidget<UAFA_OrderWidget>(this, OrderWidgetClass);
	//NewOrderWidget->AddToViewport();

	UAFA_ToyVerifier* NewOrderCDO = Cast<UAFA_ToyVerifier>(NewOrder->GetDefaultObject());
	if (!ensure(NewOrderCDO != nullptr))
		return;

	NewOrderWidget->InitialiseOrder(NewOrderCDO->ToyName, NewOrderCDO->OverlayMaterial);

	OrderWidgets.Add(NewOrderWidget);
	HorizontalBox->AddChild(NewOrderWidget);
}

