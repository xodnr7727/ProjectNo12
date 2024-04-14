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

	LaserSkillBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LaserSkill Box"));
	LaserSkillBox->SetupAttachment(GetRootComponent());
	LaserSkillBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaserSkillBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	LaserSkillBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	RushSkillBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RushSkill Box"));
	RushSkillBox->SetupAttachment(GetRootComponent());
	RushSkillBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RushSkillBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	RushSkillBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	ClawSkillBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ClawSkill Box"));
	ClawSkillBox->SetupAttachment(GetRootComponent());
	ClawSkillBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ClawSkillBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	ClawSkillBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	TeethSkillBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TeethSkill Box"));
	TeethSkillBox->SetupAttachment(GetRootComponent());
	TeethSkillBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TeethSkillBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	TeethSkillBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	LeftCastSkillBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCastSkill Box"));
	LeftCastSkillBox->SetupAttachment(GetRootComponent());
	LeftCastSkillBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftCastSkillBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	LeftCastSkillBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

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

	LaserSkillEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LaserSkillEffect"));
	LaserSkillEffect->SetupAttachment(RootComponent); // 이펙트 위치 설정

	LeftCastEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LeftCastEffect"));
	LeftCastEffect->SetupAttachment(RootComponent); // 이펙트 위치 설정

	WeaponSpellAttackEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WeaponSpellAttackEffect"));
	WeaponSpellAttackEffect->SetupAttachment(GetRootComponent());

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

void AWeapon::PlayLaserSkillSound()
{
	if (LaserSkillSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			LaserSkillSound,
			GetActorLocation()
		);
	}
}

void AWeapon::SpawnLaserSkillHitParticles(const FVector& ImpactPoint)
{
	if (LaserSkillHitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			LaserSkillHitParticles,
			ImpactPoint
		);
	}
}

void AWeapon::PlayRushSkillSound()
{
	if (RushSkillSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			RushSkillSound,
			GetActorLocation()
		);
	}
}

void AWeapon::SpawnRushSkillHitParticles(const FVector& ImpactPoint)
{
	if (RushSkillHitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			RushSkillHitParticles,
			ImpactPoint
		);
	}
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

void AWeapon::DeactivateLaserSkillEffect()
{
	if (LaserSkillEffect)
	{
		LaserSkillEffect->DeactivateSystem();
	}
}

void AWeapon::ActivateLaserSkillEffect()
{
	if (LaserSkillEffect)
	{
		LaserSkillEffect->ActivateSystem();
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


void AWeapon::DeactivateLeftCastSkillEffect()
{
	if (LeftCastEffect)
	{
		UE_LOG(LogTemp, Log, TEXT("DeactivateLeft"));
		LeftCastEffect->DeactivateSystem();
	}
}

void AWeapon::ActivateLeftCastSkillEffect()
{
	if (LeftCastEffect)
	{
		LeftCastEffect->ActivateSystem();
	}
}

void AWeapon::DeactivateWeaponSpellEffect()
{
	if (WeaponSpellAttackEffect)
	{
		WeaponSpellAttackEffect->Deactivate();
	}
}

void AWeapon::ActivateWeaponSpellEffect()
{
	if (WeaponSpellAttackEffect)
	{
		WeaponSpellAttackEffect->Activate();
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
	if (LargeSkillEffect) {
		LargeSkillEffect->DeactivateSystem(); // 초기에는 비활성화
	}
	if (SmallSkillEffect) {
		SmallSkillEffect->DeactivateSystem(); // ''
	}
	if (GuardCounterEffect) {
		GuardCounterEffect->DeactivateSystem(); // ''
	}
	if (LaserSkillEffect) {
		LaserSkillEffect->DeactivateSystem(); // ''
	}
	if (LeftCastEffect) {
		LeftCastEffect->DeactivateSystem(); // ''
	}
	if (WeaponSpellAttackEffect) {
		WeaponSpellAttackEffect->Deactivate();
	}

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
	SkillBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSkillBoxOverlap);
	LaserSkillBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnLaserSkillBoxOverlap);
	RushSkillBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnRushSkillBoxOverlap);
	TeethSkillBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
	ClawSkillBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
	LeftCastSkillBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
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

void AWeapon::IncreaseLaserSkillDamage()
{
	// 공격력을 증가시키는 코드
	Damage += 50.0f;
}

void AWeapon::RestoreLaserSkillDamage()
{
	// 공격력을 원래대로 복구시키는 코드
	// 예를 들어, CharacterDamage = BaseDamage;
	Damage -= 50.0f;
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

void AWeapon::OnLaserSkillBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	FHitResult BoxHit;
	BoxTrace(BoxHit);

	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(BoxHit.GetActor());
	ALichEnemy* HitEnemy = Cast<ALichEnemy>(BoxHit.GetActor());
	AGoblin* Goblin = Cast<AGoblin>(BoxHit.GetActor());
	ABossCharacter* BossCharacter = Cast<ABossCharacter>(BoxHit.GetActor());
	AActor* OwnerActor = GetOwner();

	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(ALichEnemy::StaticClass()) && GetOwner() && GetOwner()->IsA(ALichEnemy::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(ALichEnemy::StaticClass()) && GetOwner() && GetOwner()->IsA(ABossCharacter::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(ALichEnemy::StaticClass()) && GetOwner() && GetOwner()->IsA(AGoblin::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(ABossCharacter::StaticClass()) && GetOwner() && GetOwner()->IsA(ABossCharacter::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(ABossCharacter::StaticClass()) && GetOwner() && GetOwner()->IsA(ALichEnemy::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(ABossCharacter::StaticClass()) && GetOwner() && GetOwner()->IsA(AGoblin::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(AGoblin::StaticClass()) && GetOwner() && GetOwner()->IsA(ABossCharacter::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(AGoblin::StaticClass()) && GetOwner() && GetOwner()->IsA(ALichEnemy::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(AGoblin::StaticClass()) && GetOwner() && GetOwner()->IsA(AGoblin::StaticClass())) return;
	if (BoxHit.GetActor() != GetOwner())
	{
		UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		ExecuteGetHit(BoxHit);
		ExecuteGetBlock(BoxHit);
		CreateFields(BoxHit.ImpactPoint);
		SpawnLaserSkillHitParticles(BoxHit.ImpactPoint);
		PlayLaserSkillSound(); //스킬 타격 사운드 재생

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

void AWeapon::OnRushSkillBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	FHitResult BoxHit;
	BoxTrace(BoxHit);

	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(BoxHit.GetActor());
	ALichEnemy* HitEnemy = Cast<ALichEnemy>(BoxHit.GetActor());
	AGoblin* Goblin = Cast<AGoblin>(BoxHit.GetActor());
	ABossCharacter* BossCharacter = Cast<ABossCharacter>(BoxHit.GetActor());
	AActor* OwnerActor = GetOwner();

	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(ALichEnemy::StaticClass()) && GetOwner() && GetOwner()->IsA(ALichEnemy::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(ALichEnemy::StaticClass()) && GetOwner() && GetOwner()->IsA(ABossCharacter::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(ALichEnemy::StaticClass()) && GetOwner() && GetOwner()->IsA(AGoblin::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(ABossCharacter::StaticClass()) && GetOwner() && GetOwner()->IsA(ABossCharacter::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(ABossCharacter::StaticClass()) && GetOwner() && GetOwner()->IsA(ALichEnemy::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(ABossCharacter::StaticClass()) && GetOwner() && GetOwner()->IsA(AGoblin::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(AGoblin::StaticClass()) && GetOwner() && GetOwner()->IsA(ABossCharacter::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(AGoblin::StaticClass()) && GetOwner() && GetOwner()->IsA(ALichEnemy::StaticClass())) return;
	if (BoxHit.GetActor() && BoxHit.GetActor()->IsA(AGoblin::StaticClass()) && GetOwner() && GetOwner()->IsA(AGoblin::StaticClass())) return;
	if (BoxHit.GetActor() != GetOwner())
	{
		UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		ExecuteGetHit(BoxHit);
		ExecuteGetBlock(BoxHit);
		CreateFields(BoxHit.ImpactPoint);
		SpawnRushSkillHitParticles(BoxHit.ImpactPoint);
		PlayRushSkillSound(); //스킬 타격 사운드 재생

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
