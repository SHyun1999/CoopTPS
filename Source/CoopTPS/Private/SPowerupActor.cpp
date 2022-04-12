// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
	PowerupInterval = 0.f;
	NumberOfTicks = 0;
}

// Called when the game starts or when spawned
void ASPowerupActor::BeginPlay()
{
	Super::BeginPlay();

}

void ASPowerupActor::OnTickPowerup()
{
	TicksProcessed++;

	if (FinishedTicking())
	{
		OnExpired();

		GetWorldTimerManager().ClearTimer(TimerHandlePowerupTick);
	}

}

bool ASPowerupActor::IsOverTime()
{
	return PowerupInterval > 0;
}

bool ASPowerupActor::FinishedTicking()
{
	return TicksProcessed >= NumberOfTicks;
}

void ASPowerupActor::ActivatePowerup()
{

	OnActivated();

	if (IsOverTime())
	{
		GetWorldTimerManager().SetTimer(TimerHandlePowerupTick, this, &ASPowerupActor::OnTickPowerup, true);
	}
	else
	{
		OnTickPowerup();
	}
}


