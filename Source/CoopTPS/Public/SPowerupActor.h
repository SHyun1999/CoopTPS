// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

UCLASS()
class COOPTPS_API ASPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupActor();

protected:

	//time between powerup ticks
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float PowerupInterval;

	//times we apply powerup
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 NumberOfTicks;

	FTimerHandle TimerHandlePowerupTick;

	//total numbers of ticks applied
	int32 TicksProcessed;

	UPROPERTY(ReplicatedUsing=OnRep_PowerupActive)
	bool bIsPowerupActive;

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp")
	void OnPowerupStateChanged( bool bNewIsActive);

	UFUNCTION()
	void OnRep_PowerupActive();

	UFUNCTION()
	void OnTickPowerup();

	bool IsOverTime();

	bool FinishedTicking();

public:	

	void ActivatePowerup(AActor* ActiveFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp")
	void OnActivated(AActor* ActiveFor);

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp")
	void OnExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp")
	void OnPowerupTicked();

};
