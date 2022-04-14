// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_SixParams(FOnHealthChangedSignature, USHealthComponent, OnHealthChanged, USHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, class AActor*, DamageCauser);

UCLASS( ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class COOPTPS_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealthComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category ="HealthComponent")
	uint8 TeamNum;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "HealthComponent")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthComponent")
	float DefaultHealth;

	UFUNCTION()
	void OnRep_Health(float OldHealth);

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category = "HealthComp")
	void Heal(float HealAmount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HealthComp")
	static bool IsFriendly(AActor* ActorA, AActor* ActorB);

	float GetHealth() const;
};
