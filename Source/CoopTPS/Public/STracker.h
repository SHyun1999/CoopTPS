// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STracker.generated.h"

class USHealthComponent;
class USphereComponent;
class USoundCue;

UCLASS()
class COOPTPS_API ASTracker : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTracker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* ExplosionFX;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	USoundCue* SelfDestSFX;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	USoundCue* ExplodeSFX;

	//next point in navigation path
	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "Tracker")
	bool bUseVelocityChange; 

	UPROPERTY(EditDefaultsOnly, Category = "Tracker")
	float MinDistanceToTarget;

	UMaterialInstanceDynamic* MatInst;

	bool bExploded;

	bool bStartedSelfDest;

	FTimerHandle TimerHandle_SelfDamage;

	void SelfDestruct();
	
	FVector GetNextPathPoint();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthCompThing, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, class AActor* DamageCauser);

	void DamageSelf();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
