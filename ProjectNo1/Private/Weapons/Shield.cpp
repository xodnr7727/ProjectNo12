// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Shield.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/Weapon.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "Characters/BaseCharacter.h"
#include "NiagaraComponent.h"

// Sets default values
AShield::AShield()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ShieldBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ShieldBox"));
	ShieldBox->SetupAttachment(GetRootComponent());
	ShieldBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ShieldBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	ShieldBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

}

// Called when the game starts or when spawned
void AShield::BeginPlay()
{
	Super::BeginPlay();
	
	ShieldBox->OnComponentBeginOverlap.AddDynamic(this, &AShield::OnBoxOverlap);
}


// Called every frame
void AShield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShield::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	DisableCapsuleCollision();
	DeactivateEmbers();
}
void AShield::DeactivateEmbers()
{
	if (ItemEffect)
	{
		ItemEffect->Deactivate();
	}
}

void AShield::DisableCapsuleCollision()
{
	if (Capsule)
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
void AShield::OnWeaponHit(AActor* OtherWeapon)
{

	FHitResult BoxHit;
	if (BoxHit.GetActor() != GetOwner() && OtherWeapon && OtherWeapon != this){

		if (BlockSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				BlockSound,
				GetActorLocation()
			);
			if (HitParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					this,
					HitParticles,
					GetActorLocation()
				);
			}
		}
}
}
void AShield::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AShield::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	FHitResult BoxHit;
	AWeapon* OtherWeapon = Cast<AWeapon>(OtherActor);

	if (BoxHit.GetActor() != GetOwner())
	{
		ExecuteGetHit(BoxHit);
		if (BoxHit.GetActor() == OtherWeapon) {
			OnWeaponHit(OtherWeapon);
		}
	}
}
void AShield::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	}
}