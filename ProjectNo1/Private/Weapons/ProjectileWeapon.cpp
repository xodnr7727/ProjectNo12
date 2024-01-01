// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/HitInterface.h"
#include "Characters/BaseCharacter.h"
#include "ProjectNo1/LichEnemy.h"
#include "BossCharacter.h"
#include "Goblin.h"
#include "Math/Vector.h"
#include "Engine/World.h"
#include <Kismet/KismetMathLibrary.h>

// Sets default values
AProjectileWeapon::AProjectileWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(10.0f);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectileWeapon::OnBoxOverlap);
	CollisionComponent->SetupAttachment(GetRootComponent());;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 800.0f;
	ProjectileMovement->MaxSpeed = 800.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	InitialLifeSpan = 3.0f;

	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));
	SwordMesh->SetupAttachment(GetRootComponent());;

	ImpactEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ImpactEffect"));
	ImpactEffect->SetupAttachment(RootComponent);

	Damage = 100.0f;  // Set a default damage amount
}

void AProjectileWeapon::ThrowInDirection(const FVector& ShootDirection)
{
	ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
}

void AProjectileWeapon::ProjectileEquip(USceneComponent* InParent, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	DisableCapsuleCollision();
}

void AProjectileWeapon::ExecuteBallHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_BallHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	}
}

void AProjectileWeapon::DisableCapsuleCollision()
{
	if (Capsule)
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

// Called when the game starts or when spawned
void AProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProjectileWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FHitResult BoxHit;

	ALichEnemy* HitEnemy = Cast<ALichEnemy>(BoxHit.GetActor());

	if (OtherActor && OtherActor != this && OtherComp)
	{
		if (OtherActor->IsA(ALichEnemy::StaticClass()))
		{
			ExecuteBallHit(BoxHit);
			if (HitEnemy) {
				HitEnemy->ShowHitNumber(Damage, BoxHit.Location);
			}
			UE_LOG(LogTemp, Log, TEXT("DestroyActor"));
			Destroy();
		}
	}
}

void AProjectileWeapon::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FHitResult BoxHit;

	ALichEnemy* HitEnemy = Cast<ALichEnemy>(BoxHit.GetActor());
	AGoblin* Goblin = Cast<AGoblin>(BoxHit.GetActor());
	ABossCharacter* BossCharacter = Cast<ABossCharacter>(BoxHit.GetActor());
	AActor* OwnerActor = GetOwner();

	// 몬스터에게 타격을 입힐 때 호출되는 함수
	if (BoxHit.GetActor() != GetOwner())
	{
	    UGameplayStatics::ApplyPointDamage(BoxHit.GetActor(), Damage, NormalImpulse, Hit, GetInstigatorController(), this, UDamageType::StaticClass());
		ExecuteBallHit(BoxHit);
		UE_LOG(LogTemp, Log, TEXT("EnemyHit"));

		if (HitEnemy) {
			HitEnemy->ShowHitNumber(Damage, BoxHit.Location);
		}
		if (Goblin) {
			Goblin->ShowHitNumber(Damage, BoxHit.Location);
		}
		if (BossCharacter) {
			BossCharacter->ShowHitNumber(Damage, BoxHit.Location);
		}
		Destroy();
	}
}



