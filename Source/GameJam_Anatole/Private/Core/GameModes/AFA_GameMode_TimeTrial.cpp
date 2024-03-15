// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameModes/AFA_GameMode_TimeTrial.h"

void AAFA_GameMode_TimeTrial::BeginPlay()
{
	Super::BeginPlay();

	TimerValue = TimerMax;
}

void AAFA_GameMode_TimeTrial::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isTimerStarted)
	{
		TimerValue -= DeltaTime;

		if (TimerValue <= 0)
		{
			OnTimerEnd();
			isTimerStarted = false;
			TimerValue = 0;
		}
	}
}

void AAFA_GameMode_TimeTrial::StopGame()
{
	Super::StopGame();

	// TODO: show end screen widget + disable input
}

void AAFA_GameMode_TimeTrial::StartTimer()
{
	isTimerStarted = true;
}

void AAFA_GameMode_TimeTrial::OnTimerEnd()
{
	StopGame();
}
