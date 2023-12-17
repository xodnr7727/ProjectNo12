// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileWeapon.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "Engine/Classes/Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/HitInterface.h"
#include "Characters/BaseCharacter.h"
#include "ProjectNo1/LichEnemy.h"
#include "BossCharacter.h"
#include "Goblin.h"
#include "Math/Vector.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include <Kismet/KismetMathLibrary.h>

// Sets default values
AProjectileWeapon::AProjectileWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));
	RootComponent = SwordMesh;

	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(RootComponent);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	// �浹 �̺�Ʈ ���ε�
	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectileWeapon::OnHit);
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

void AProjectileWeapon::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// ���Ϳ��� Ÿ���� ���� �� ȣ��Ǵ� �Լ�
	if (OtherActor && OtherActor->IsA(ALichEnemy::StaticClass()))
	{
		ALichEnemy* LichEnemy = Cast<ALichEnemy>(OtherActor);
		if (LichEnemy)
		{
			// ���Ϳ��� �������� ������ �ڵ带 ���⿡ �߰�
			LichEnemy->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);

		}
	}else if (OtherActor && OtherActor->IsA(AGoblin::StaticClass()))
	{
		AGoblin* Goblin = Cast<AGoblin>(OtherActor);
		if (Goblin)
		{
			// ���Ϳ��� �������� ������ �ڵ带 ���⿡ �߰�
			Goblin->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);

		}
	}else if (OtherActor && OtherActor->IsA(ABossCharacter::StaticClass()))
	{
		ABossCharacter* BossCharacter = Cast<ABossCharacter>(OtherActor);
		if (BossCharacter)
		{
			// ���Ϳ��� �������� ������ �ڵ带 ���⿡ �߰�
			BossCharacter->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);

		}
	}

}

