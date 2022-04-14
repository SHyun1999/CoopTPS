// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SHordeGameMode.generated.h"

/**
 * 
 */
enum class EWaveState : uint8;

UCLASS()
class COOPTPS_API ASHordeGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:

	//nr of enemies to spawn in current wave
	int32 EnemiesToSpawn;

	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category ="Gamemode")
	float TimeBetweenWaves;

	FTimerHandle TimerHandle_NextWave;

	FTimerHandle TimerHandle_EnemySpawner;
	//hook in bp to spawn a tracker enemy
	UFUNCTION(BlueprintImplementableEvent, Category = "Gamemode")
	void SpawnNewEnemy();

	void SpawnBotTimeElapsed();

	//start spawning enemies
	void StartWave();

	//stop spawning enemies
	void EndWave();

	//set timer for next wave
	void PrepNextWave();

	void CheckWaveState();

	void CheckPlayersAlive();

	void GameOver();

	void SetWaveState(EWaveState NewState);

public:
	ASHordeGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;
};
