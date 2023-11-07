// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Weapon.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/HitInterface.h"
#include "Characters/BaseCharacter.h"
#include "Math/Vector.h"
#include "ProjectNo1/LichEnemy.h"
#include "BossCharacter.h"
#include "Goblin.h"
#include "NiagaraComponent.h"
#include "Weapons/Shield.h"
#include <Kismet/KismetMathLibrary.h>

AWeapon::AWeapon()
{
	SwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwordMesh"));
	SwordMesh->SetupAttachment(GetRootComponent());

	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());

	FVector WeaponLocation = FVector(0.0f, 0.0f, 0.0f); // ���ϴ� ��ġ�� ����
	FRotator WeaponRotation = FRotator(0.0f, 0.0f, 0.0f); // ���ϴ� ȸ������ ����

	ItemMesh->SetRelativeLocation(WeaponLocation);
	ItemMesh->SetRelativeRotation(WeaponRotation);

	LargeSkillEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LargeSkillEffect"));
	LargeSkillEffect->SetupAttachment(RootComponent); // ����Ʈ ��ġ ����

}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	PlayEquipSound();
	DisableCapsuleCollision();
	DeactivateEmbers();
}

void AWeapon::DeactivateLargeSkillEffect()
{
	if (LargeSkillEffect)
	{
		LargeSkillEffect->DeactivateSystem();
	}
}

void AWeapon::ActivateLargeSkillEffect()
{
	if (LargeSkillEffect)
	{
		LargeSkillEffect->ActivateSystem();
	}
}

void AWeapon::DeactivateEmbers()
{
	if (ItemEffect)
	{
		ItemEffect->Deactivate();
	}
}

void AWeapon::DisableCapsuleCollision()
{
	if (Capsule)
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquipSound,
			GetActorLocation()
		);
	}
}

void AWeapon::FireballSword()
{
	// Į�� ��ġ�� ȸ�� ���� �����ɴϴ�.
	FVector SwordLocation = SwordMesh->GetSocketLocation("SwordEnd");
	FRotator SwordRotation = SwordMesh->GetSocketRotation("SwordEnd");

	// ���̾�� �����ϱ� ���� ���̾ Ŭ������ �ν��Ͻ��� �����մϴ�.
	AProjectileWeapon* ProjectileWeapon = GetWorld()->SpawnActor<AProjectileWeapon>(ProjectileWeaponClass, SwordLocation, SwordRotation);

	if (ProjectileWeapon)
	{
		// ���̾�� ������ �������� ���������ϴ�.
		FVector FireballDirection = GetActorForwardVector() + FVector(0.0f, 0.0f, -1.0f);
		ProjectileWeapon->LaunchFireball(FireballDirection);
	}
}


void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	LargeSkillEffect->DeactivateSystem(); // �ʱ⿡�� ��Ȱ��ȭ

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::IncreaseDamage()
{
	// ���ݷ��� ������Ű�� �ڵ�
	Damage += 100.0f;
}

void AWeapon::RestoreDamage()
{
	// ���ݷ��� ������� ������Ű�� �ڵ�
	// ���� ���, CharacterDamage = BaseDamage;
	Damage -= 100.0f;
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	FHitResult BoxHit;
	BoxTrace(BoxHit);

	ALichEnemy* HitEnemy = Cast<ALichEnemy>(BoxHit.GetActor());
	AGoblin* Goblin = Cast<AGoblin>(BoxHit.GetActor());
	ABossCharacter* BossCharacter = Cast<ABossCharacter>(BoxHit.GetActor());
	AActor* OwnerActor = GetOwner();

	if(BoxHit.GetActor() && BoxHit.GetActor()->IsA(ALichEnemy::StaticClass()) && GetOwner() && GetOwner()->IsA(ALichEnemy::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(ALichEnemy::StaticClass()) && GetOwner() && GetOwner()->IsA(ABossCharacter::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(ALichEnemy::StaticClass()) && GetOwner() && GetOwner()->IsA(AGoblin::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(ABossCharacter::StaticClass()) && GetOwner() && GetOwner()->IsA(ABossCharacter::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(ABossCharacter::StaticClass()) && GetOwner() && GetOwner()->IsA(ALichEnemy::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(ABossCharacter::StaticClass()) && GetOwner() && GetOwner()->IsA(AGoblin::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(AGoblin::StaticClass()) && GetOwner() && GetOwner()->IsA(ABossCharacter::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(AGoblin::StaticClass()) && GetOwner() && GetOwner()->IsA(ALichEnemy::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(AGoblin::StaticClass()) && GetOwner() && GetOwner()->IsA(AGoblin::StaticClass())) return;
	//�´� ��� �����̰� ������ �����ֵ� �����϶� ����
		if (BoxHit.GetActor() != GetOwner())
		{
			UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
			ExecuteGetHit(BoxHit);
			ExecuteGetBlock(BoxHit);
			CreateFields(BoxHit.ImpactPoint);

			if (HitEnemy) {
				HitEnemy->ShowHitNumber(Damage, BoxHit.Location);
			}
			if (Goblin) {
				Goblin->ShowHitNumber(Damage, BoxHit.Location);		
		    }
			if (BossCharacter) {
				BossCharacter->ShowHitNumber(Damage, BoxHit.Location);
			}
		}
}

bool AWeapon::CanShieldBlock()
{
	return WeaponAState == EWeaponAState::EIS_Attaking;
}

void AWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	}
}
void AWeapon::ExecuteGetBlock(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetBlock(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	}
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());
	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

UKismetSystemLibrary::BoxTraceSingle(
	this,
	Start,
	End,
	BoxTraceExtent,
	BoxTraceStart->GetComponentRotation(),
	ETraceTypeQuery::TraceTypeQuery1,
	false,
	ActorsToIgnore,
	bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
	BoxHit,
	true
);
IgnoreActors.AddUnique(BoxHit.GetActor());
}
