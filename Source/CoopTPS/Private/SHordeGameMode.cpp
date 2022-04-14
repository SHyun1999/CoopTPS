// Fill out your copyright notice in the Description page of Project Settings.


#include "SHordeGameMode.h"
#include "TimerManager.h"
#include "SHealthComponent.h"
#include "SGameState.h"


ASHordeGameMode::ASHordeGameMode()
{

	TimeBetweenWaves = 10.f;

	GameStateClass = ASGameState::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f;

}

void ASHordeGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaveState();
	CheckPlayersAlive();

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

	SetWaveState(EWaveState::WaveInProgress);

	GetWorldTimerManager().SetTimer(TimerHandle_EnemySpawner, this, &ASHordeGameMode::SpawnBotTimeElapsed, 1.f, true, 0.f);

}

void ASHordeGameMode::EndWave()
{

	SetWaveState(EWaveState::WaveComplete);
	GetWorldTimerManager().ClearTimer(TimerHandle_EnemySpawner);

}

void ASHordeGameMode::PrepNextWave()
{
	SetWaveState(EWaveState::WaitingToStart);
	GetWorldTimerManager().SetTimer(TimerHandle_NextWave, this, &ASHordeGameMode::StartWave, TimeBetweenWaves, true, 0.f);
}

void ASHordeGameMode::CheckWaveState()
{

	bool bIsPrepingWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWave);

	if (EnemiesToSpawn > 0 || bIsPrepingWave) return;

	bool bIsAnyEnemyLeft = false;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* Pawn = It->Get();

		if (Pawn == nullptr || Pawn->IsPlayerControlled())
		{
			continue;
		}

		USHealthComponent* HealthComp = Cast<USHealthComponent>(Pawn->GetComponentByClass(USHealthComponent::StaticClass()));


		if (HealthComp && HealthComp->GetHealth() > 0.f)
		{
			bIsAnyEnemyLeft = true;
			break;
		}

		if (!bIsAnyEnemyLeft)
		{
			PrepNextWave();
		}
	}
	
}

void ASHordeGameMode::CheckPlayersAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();

		if (PC && PC->GetPawn())
		{
			APawn* Pawn = PC->GetPawn();
			USHealthComponent* HealthComp = Cast<USHealthComponent>(Pawn->GetComponentByClass(USHealthComponent::StaticClass()));

			if (HealthComp && HealthComp->GetHealth() > 0.f)
			{
				return; //a player is alive.
			}
		}
	}

	GameOver();
}

void ASHordeGameMode::GameOver()
{
	EndWave();

	SetWaveState(EWaveState::GameOver);
	UE_LOG(LogTemp, Warning, TEXT("lowoser"));
	// TODO end game.
}

void ASHordeGameMode::SetWaveState(EWaveState NewState)
{

	ASGameState* GS = GetGameState<ASGameState>();

	if (GS)
	{
		GS->WaveState = NewState;
	}
}

void ASHordeGameMode::StartPlay()
{
	Super::StartPlay();

	PrepNextWave();

}
