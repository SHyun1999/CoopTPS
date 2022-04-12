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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//time between powerup ticks
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float PowerupInterval;

	//times we apply powerup
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 NumberOfTicks;

	FTimerHandle TimerHandlePowerupTick;

	//total numbers of ticks applied
	int32 TicksProcessed;

	UFUNCTION()
	void OnTickPowerup();

	bool IsOverTime();

	bool FinishedTicking();

public:	

	void ActivatePowerup();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp")
	void OnActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp")
	void OnExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "PowerUp")
	void OnPowerupTicked();

};
