// Fill out your copyright notice in the Description page of Project Settings.


#include "STracker.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "SHealthComponent.h"
#include "SCharacter.h"
#include "Sound/SoundCue.h"
// Sets default values
ASTracker::ASTracker()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTracker::OnHealthChanged);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(600);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);

	bUseVelocityChange = true;
	MovementForce = 1000;
	MinDistanceToTarget = 100;

}

// Called when the game starts or when spawned
void ASTracker::BeginPlay()
{
	Super::BeginPlay();

	
	if (HasAuthority())
	{
		NextPathPoint = GetNextPathPoint();
	}

}

// Called every frame
void ASTracker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && !bExploded)
	{

		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

		if (DistanceToTarget <= MinDistanceToTarget)
		{
			NextPathPoint = GetNextPathPoint();
		}
		else
		{
			//move to next target

			FVector ForceDirection = NextPathPoint - GetActorLocation();

			ForceDirection.Normalize();
			ForceDirection *= MovementForce;

			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
		}

	}

}

void ASTracker::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (bStartedSelfDest) return;

	ASCharacter* PlayerChar = Cast<ASCharacter>(OtherActor);

	if (PlayerChar)
	{
		bStartedSelfDest = true;
		
		UGameplayStatics::SpawnSoundAttached(SelfDestSFX, RootComponent);

		if (HasAuthority())
		{
			GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTracker::DamageSelf, 0.5f, true, 0.f);
		}


	}

}


void ASTracker::SelfDestruct()
{
	if (bExploded) return;

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSFX, GetActorLocation());

	MeshComp->SetVisibility(false, true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (HasAuthority())
	{

		TArray<AActor*> IgnoredActors;

		UGameplayStatics::ApplyRadialDamage(this, 100, GetActorLocation(), 200, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		SetLifeSpan(2.f);
}
	}

FVector ASTracker::GetNextPathPoint()
{
	ACharacter* PlayerPawn =  UGameplayStatics::GetPlayerCharacter(this, 0);
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}

	return GetActorLocation();

}

void ASTracker::OnHealthChanged(USHealthComponent* HealthCompThing, float Health, float HealthDelta,
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0)
	{
		SelfDestruct();
	}
	
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0,MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastDamageTaken", GetWorld()->TimeSeconds);
	}


}

void ASTracker::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 40, GetInstigatorController(), this, nullptr);
}
