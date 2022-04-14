// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	DefaultHealth = 100.f;

	TeamNum = 255;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	//only hook if we are server
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
		Health = DefaultHealth;
	}

}

void USHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;

	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{

	if (Damage <= 0.f) return;

	if (DamageCauser != DamagedActor && IsFriendly(DamagedActor, DamageCauser)) return;

	Health = FMath::Clamp(Health - Damage, 0.f, DefaultHealth);

	UE_LOG(LogTemp, Warning, TEXT("%f"), Health);

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

}

void USHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.f || Health <= 0.f) return;

	Health = FMath::Clamp(Health + HealAmount, 0.f, DefaultHealth);
	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));

}

bool USHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		// assume friendly
		return true;
	}

	USHealthComponent* HealthCompA = Cast<USHealthComponent>(ActorA->GetComponentByClass(USHealthComponent::StaticClass()));
	USHealthComponent* HealthCompB = Cast<USHealthComponent>(ActorB->GetComponentByClass(USHealthComponent::StaticClass()));

	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		// assume friendly
		return true;
	}

	return HealthCompA->TeamNum == HealthCompB->TeamNum;

}

float USHealthComponent::GetHealth() const
{
	return Health;
}

///////////////////////////////SERVER

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, Health);
}