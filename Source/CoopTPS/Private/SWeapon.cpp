// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include"PhysicalMaterials/PhysicalMaterial.h"
#include "CoopTPS/CoopTPS.h"
#include "TimerManager.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing, 
	TEXT("Draw Debug Lines for weapons"),
	ECVF_Cheat);
// Sets default values
ASWeapon::ASWeapon()
{

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	RateOfFire = 300;

	BaseDamage = 20.f;

}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	TimeBetweenShots = 60 / RateOfFire;

}

void ASWeapon::Fire()
{
	AActor* MyOwner = GetOwner();

	if (!MyOwner) return;
	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector ShotDirection = EyeRotation.Vector();
	FVector TraceEnd = EyeLocation + (ShotDirection * 10000);



	FCollisionQueryParams QueryParams;

	QueryParams.AddIgnoredActor(MyOwner);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	FVector TracerEndPoint = TraceEnd;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
	{
		AActor* HitActor = Hit.GetActor();

		EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
		
		float DamageToApply = BaseDamage;

		if (SurfaceType == SURFACE_FLESHVULNERABLE)
		{
			DamageToApply *= 2.5f;
		}

		UGameplayStatics::ApplyPointDamage(HitActor, DamageToApply, ShotDirection,Hit,MyOwner->GetInstigatorController(), this, DamageType);

		UParticleSystem* SelectedEffect = nullptr;
		switch (SurfaceType)
		{
		case SURFACE_FLESHDEFAULT:
		case SURFACE_FLESHVULNERABLE:
			SelectedEffect = FleshImpactEffect;
			break;
		default:
			SelectedEffect = DefaultImpactEffect;
			break;
		}

		if (SelectedEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
		}
		TracerEndPoint = Hit.ImpactPoint;
	}

	if (DebugWeaponDrawing > 0)
	{
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.f, 0, 1.f);
	}

	PlayFireEffect(TracerEndPoint);
	ShakeCamera(MyOwner);

	LastFireTime = GetWorld()->TimeSeconds;
}

void ASWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.f);
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, 0.25f, true, FirstDelay);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::PlayFireEffect(FVector TracerEndPoint)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect,MeshComp, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);

		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
		}
	}
}

void ASWeapon::ShakeCamera(AActor* MyOwner)
{

	APawn* MyOwnerPawn = Cast<APawn>(MyOwner);

	APlayerController* PC = Cast<APlayerController>(MyOwnerPawn->GetController());
	if (PC)
	{
		PC->ClientPlayCameraShake(FireCamShake);
	}
}

