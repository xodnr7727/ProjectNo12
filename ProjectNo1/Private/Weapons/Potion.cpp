// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Potion.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/Weapon.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "ProjectNo1/LichEnemy.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "Characters/BaseCharacter.h"
#include "NiagaraComponent.h"

// Sets default values
APotion::APotion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PotionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PotionMesh"));
	PotionMesh->SetupAttachment(GetRootComponent());

	PotionEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PotionEffect"));
	PotionEffect->SetupAttachment(RootComponent); // 이펙트 위치 설정

	PotionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PotionBox"));
	PotionBox->SetupAttachment(GetRootComponent());
	PotionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PotionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	PotionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

// Called when the game starts or when spawned
void APotion::BeginPlay()
{
	Super::BeginPlay();
	
	if (ItemEffect) {
		ItemEffect->Deactivate(); // 초기에는 비활성화
	}
}

void APotion::DeactivateEffect()
{
	if (ItemEffect)
	{
		ItemEffect->Deactivate();
	}
}

void APotion::ActivateEffect()
{
	if (ItemEffect)
	{
		ItemEffect->Activate();
	}
}

void APotion::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	DisableCapsuleCollision();
}

void APotion::DisableCapsuleCollision()
{
	if (Capsule)
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void APotion::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

// Called every frame
void APotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

