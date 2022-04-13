// Fill out your copyright notice in the Description page of Project Settings.


#include "SHordeGameMode.h"
#include "TimerManager.h"


ASHordeGameMode::ASHordeGameMode()
{

	TimeBetweenWaves = 2.f;

}

void ASHordeGameMode::SpawnBotTimeElapsed()
{

	SpawnNewEnemy();

	EnemiesToSpawn--;

	if (EnemiesToSpawn <= 0)
	{
		EndWave();
	}
}

void ASHordeGameMode::StartWave()
{
	WaveCount++;

	EnemiesToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_EnemySpawner, this, &ASHordeGameMode::SpawnBotTimeElapsed, 1.f, true, 0.f);

}

void ASHordeGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_EnemySpawner);

	PrepNextWave();
}

void ASHordeGameMode::PrepNextWave()
{
	FTimerHandle TimerHandle_NextWave;
	GetWorldTimerManager().SetTimer(TimerHandle_NextWave, this, &ASHordeGameMode::StartWave, TimeBetweenWaves, true, 0.f);
}

void ASHordeGameMode::StartPlay()
{
	Super::StartPlay();

	PrepNextWave();

}
