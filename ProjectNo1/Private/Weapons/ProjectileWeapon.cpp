// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileWeapon.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "Engine/Classes/Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ProjectNo1/LichEnemy.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include <Kismet/KismetMathLibrary.h>

// Sets default values
AProjectileWeapon::AProjectileWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());
	Mesh->SetRelativeRotation(FRotator(-90.f, -90.f, 0.f));


	Particle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	Particle->SetupAttachment(GetRootComponent());

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(5.0f);
	SphereComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	SphereComponent->SetupAttachment(GetRootComponent());

	InitialLifeSpan = 3.0f;
	FireballSpeed = 1000.0f;
}

// Called when the game starts or when spawned
void AProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentHit.AddDynamic(this, &AProjectileWeapon::OnHit);
}


// Called every frame
void AProjectileWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation() + GetActorForwardVector() * FireballSpeed * DeltaTime;
	SetActorLocation(NewLocation);
}


void AProjectileWeapon::LaunchFireball(const FVector& FireballDirection)
{
	// Set the fireball's rotation to match the specified direction
	FRotator NewRotation = FireballDirection.Rotation();
	SetActorRotation(NewRotation);
}

void AProjectileWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	AProjectNo1Character* Character = Cast<AProjectNo1Character>(GetOwner());

	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, Character);
	}
}


// Overlap이 시작할 때 호출되는 함수
void AProjectileWeapon::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FHitResult BoxHit;

	ALichEnemy* HitEnemy = Cast<ALichEnemy>(BoxHit.GetActor());

	if (HitEnemy)
	{
		UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		ExecuteGetHit(BoxHit);
		CreateFields(BoxHit.ImpactPoint);

		if (HitEnemy) {
			HitEnemy->ShowHitNumber(Damage, BoxHit.Location);
		}

		this->Destroy();
	}
}



