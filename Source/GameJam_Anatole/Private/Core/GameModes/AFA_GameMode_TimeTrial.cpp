// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameModes/AFA_GameMode_TimeTrial.h"

void AAFA_GameMode_TimeTrial::BeginPlay()
{
	Super::BeginPlay();

	TimerValue = TimerMax;

	OnGameStartedDelegate.AddUObject(this, &AAFA_GameMode_TimeTrial::StartTimer);
}

void AAFA_GameMode_TimeTrial::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isTimerRunning)
	{
		TimerValue -= DeltaTime;

		if (TimerValue <= 0)
		{
			isTimerRunning = false;
			TimerValue = 0;

			OnTimerEnd();
		}
	}
}

void AAFA_GameMode_TimeTrial::StopGame()
{
	Super::StopGame();
}

void AAFA_GameMode_TimeTrial::StartTimer()
{
	isTimerRunning = true;
}

void AAFA_GameMode_TimeTrial::OnTimerEnd()
{
	StopGame();
}
