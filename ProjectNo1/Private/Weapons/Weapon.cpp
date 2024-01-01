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

	SkillBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Skill Box"));
	SkillBox->SetupAttachment(GetRootComponent());
	SkillBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkillBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	SkillBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());

	FVector WeaponLocation = FVector(0.0f, 0.0f, 0.0f); // 원하는 위치로 설정
	FRotator WeaponRotation = FRotator(0.0f, 0.0f, 0.0f); // 원하는 회전으로 설정

	ItemMesh->SetRelativeLocation(WeaponLocation);
	ItemMesh->SetRelativeRotation(WeaponRotation);

	LargeSkillEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LargeSkillEffect"));
	LargeSkillEffect->SetupAttachment(RootComponent); // 이펙트 위치 설정

	SmallSkillEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SmallSkillEffect"));
	SmallSkillEffect->SetupAttachment(RootComponent); // 이펙트 위치 설정

	GuardCounterEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("GuardCounterEffect"));
	GuardCounterEffect->SetupAttachment(RootComponent); // 이펙트 위치 설정

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

void AWeapon::PlaySkillSound()
{
	if (SkillSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			SkillSound,
			GetActorLocation()
		);
	}
}

void AWeapon::SpawnSkillHitParticles(const FVector& ImpactPoint)
{
	if (SkillHitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			SkillHitParticles,
			ImpactPoint
		);
	}
}

void AWeapon::DeactivateGuardCounterEffect()
{
	if (GuardCounterEffect)
	{
		GuardCounterEffect->DeactivateSystem();
	}
}

void AWeapon::ActivateGuardCounterEffect()
{
	if (GuardCounterEffect)
	{
		GuardCounterEffect->ActivateSystem();
	}
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

void AWeapon::DeactivateSmallSkillEffect()
{
	if (SmallSkillEffect)
	{
		SmallSkillEffect->DeactivateSystem();
	}
}

void AWeapon::ActivateSmallSkillEffect()
{
	if (SmallSkillEffect)
	{
		SmallSkillEffect->ActivateSystem();
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

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	LargeSkillEffect->DeactivateSystem(); // 초기에는 비활성화
	SmallSkillEffect->DeactivateSystem(); // ''
	GuardCounterEffect->DeactivateSystem(); // ''

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
	SkillBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSkillBoxOverlap);
}

void AWeapon::IncreaseDamage()
{
	// 공격력을 증가시키는 코드
	Damage += 100.0f;
}

void AWeapon::RestoreDamage()
{
	// 공격력을 원래대로 복구시키는 코드
	// 예를 들어, CharacterDamage = BaseDamage;
	Damage -= 100.0f;
}

void AWeapon::IncreaseSkillDamage()
{
	// 공격력을 증가시키는 코드
	Damage += 400.0f;
}

void AWeapon::RestoreSkillDamage()
{
	// 공격력을 원래대로 복구시키는 코드
	// 예를 들어, CharacterDamage = BaseDamage;
	Damage -= 400.0f;
}


void AWeapon::IncreaseStunDamage()
{
	// 공격력을 증가시키는 코드
	Damage += 500.0f;
}

void AWeapon::RestoreStunDamage()
{
	// 공격력을 원래대로 복구시키는 코드
	// 예를 들어, CharacterDamage = BaseDamage;
	Damage -= 500.0f;
}

void AWeapon::IncreaseCounterDamage()
{
	// 공격력을 증가시키는 코드
	Damage += 400.0f;
}

void AWeapon::RestoreCounterDamage()
{
	// 공격력을 원래대로 복구시키는 코드
	// 예를 들어, CharacterDamage = BaseDamage;
	Damage -= 400.0f;
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
	//맞는 대상도 몬스터이고 무기의 소유주도 몬스터일때 리턴
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

void AWeapon::OnSkillBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	FHitResult BoxHit;
	BoxTrace(BoxHit);

	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(BoxHit.GetActor());
	ALichEnemy* HitEnemy = Cast<ALichEnemy>(BoxHit.GetActor());
	AGoblin* Goblin = Cast<AGoblin>(BoxHit.GetActor());
	ABossCharacter* BossCharacter = Cast<ABossCharacter>(BoxHit.GetActor());
	AActor* OwnerActor = GetOwner();


	if (BoxHit.GetActor() != GetOwner())
	{
		UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		ExecuteGetHit(BoxHit);
		ExecuteGetBlock(BoxHit);
		CreateFields(BoxHit.ImpactPoint);
		SpawnSkillHitParticles(BoxHit.ImpactPoint);
		PlaySkillSound(); //스킬 타격 사운드 재생

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
