// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
	PowerupInterval = 0.f;
	NumberOfTicks = 0;
	
	bIsPowerupActive = false;

	SetReplicates(true);
}

void ASPowerupActor::OnRep_PowerupActive()
{
	OnPowerupStateChanged(bIsPowerupActive);
}

void ASPowerupActor::OnTickPowerup()
{
	TicksProcessed++;

	if (FinishedTicking())
	{
		OnExpired();

		bIsPowerupActive = false;
		OnRep_PowerupActive();

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

void ASPowerupActor::ActivatePowerup(AActor* ActiveFor)
{

	OnActivated(ActiveFor);

	bIsPowerupActive = true;
	OnRep_PowerupActive();

	if (IsOverTime())
	{
		GetWorldTimerManager().SetTimer(TimerHandlePowerupTick, this, &ASPowerupActor::OnTickPowerup, true);
	}
	else
	{
		OnTickPowerup();
	}
}

///////////////////SERVER//////////////

void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupActor, bIsPowerupActive);
}