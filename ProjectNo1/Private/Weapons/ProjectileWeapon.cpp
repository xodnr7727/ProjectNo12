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

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(GetRootComponent());;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	//ProjectileMovement->SetUpdatedComponent(CollisionComponent);
	ProjectileMovement->InitialSpeed = 2000.0f;
	ProjectileMovement->MaxSpeed = 2000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->Bounciness = 0.3f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	InitialLifeSpan = 3.0f;

	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));
	SwordMesh->SetupAttachment(GetRootComponent());;

	ImpactEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ImpactEffect"));
	ImpactEffect->SetupAttachment(RootComponent);

	Damage = 100.0f;  // Set a default damage amount
	MaxDistance = 10000.0f; // 10,000 units (100 meters)
}

void AProjectileWeapon::ProjectileEquip(USceneComponent* InParent, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
}

void AProjectileWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	}
}


// Called when the game starts or when spawned
void AProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();

	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectileWeapon::OnHit);
}

// Called every frame
void AProjectileWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FVector::Dist(InitialLocation, GetActorLocation()) > MaxDistance)
	{
		Destroy();
	}
	if (TargetActor)
	{
		FVector Direction = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
	}
}

void AProjectileWeapon::InitializeVelocity(FVector Direction, AActor* Target)
{
	TargetActor = Target;
	ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
}

void AProjectileWeapon::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("OnHit 이벤트 발생"));
	SpearSpellSweepTrace();

			if (HitParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, GetActorLocation());
			}
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
			}
}


void AProjectileWeapon::SpearSpellSweepTrace()
{
	UE_LOG(LogTemp, Warning, TEXT("OnHit"));
	TArray<FOverlapResult> OverlapResults;
		FVector ForwardVector = GetActorForwardVector(); // 플레이어 전방
		FVector RightVector = GetActorRightVector(); // 플레이어 측면
		FVector Start = GetActorLocation() + (ForwardVector * 200.0f);// 플레이어 앞 위치에서 시작
		FVector End = Start + ForwardVector * 100.0f; // 일정 거리만큼 트레이스
		FVector Center = this->GetActorLocation();
		FColor Color = FColor::Orange;
		FRotator PlayerRotation = GetActorRotation();
		//FHitResult HitResult;
		//TArray<FHitResult> HitResults;
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this); 
		CollisionParams.OwnerTag = "EngageableTarget";
		bool bResult = GetWorld()->OverlapMultiByChannel(
			OverlapResults,
			Center,
			FQuat::Identity,
			ECollisionChannel::ECC_Visibility,
			FCollisionShape::MakeSphere(200.0f),
			CollisionParams);

		// 트레이스 실행
		if (bResult) {
			//for (FHitResult& HitResult : HitResults) {
			for (FOverlapResult& OverlapResult : OverlapResults) {
				if (HitResult.GetActor()->ActorHasTag("Enemy"))
				{
					UE_LOG(LogTemp, Warning, TEXT("Call"));
					//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.f, 0, 1.f);
					// 트레이스의 타격점에 이펙트를 생성하여 표시
					FDamageEvent DamageEvent;

					ALichEnemy* HitEnemy = Cast<ALichEnemy>(HitResult.GetActor());
					AGoblin* Goblin = Cast<AGoblin>(HitResult.GetActor());
					ABossCharacter* BossCharacter = Cast<ABossCharacter>(HitResult.GetActor());

					UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, GetInstigatorController(), this, UDamageType::StaticClass());
					ExecuteGetHit(HitResult);
					if (HitEnemy) {
						HitEnemy->ShowHitNumber(Damage, HitResult.Location);
						HitEnemy->CombatTargetPlayer();
					}
					if (Goblin) {
						Goblin->ShowHitNumber(Damage, HitResult.Location);
						Goblin->CombatTargetPlayer();
					}
					if (BossCharacter) {
						BossCharacter->ShowHitNumber(Damage, HitResult.Location);
						BossCharacter->CombatTargetPlayer();
					}
				}
			}
		}
			//DrawDebugSphere(GetWorld(), Center, 200.0f, 16, Color, 0.5f);
			Destroy();
}



