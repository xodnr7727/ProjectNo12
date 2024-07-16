
#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Weapons/Weapon.h"
#include "Weapons/ProjectileWeapon.h"
#include "Weapons/Shield.h"
#include "Weapons/Potion.h"
#include "ProjectNo1/LichEnemy.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}



void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) //타격 받는 함수
{
	if (IsAlive() && !IsStun() && !NotEnoughStamina() && Hitter)
	{
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else if (IsAlive() && IsStun() && Hitter) {
		DirectionalHitStun(Hitter->GetActorLocation());
	}
	else if (IsAlive() && !IsStun() && NotEnoughStamina() && Hitter) {
		DirectionalHitFall(Hitter->GetActorLocation());
	}
	else Die();

	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

void ABaseCharacter::GetBlock_Implementation(const FVector& ImpactPoint, AActor* Hitter) // 방패로 막는 함수
{
     if (IsAlive() && Hitter)
	{
		HitReactBlock();
	}
	else Die();

	PlayBlockSound(ImpactPoint);
	SpawnBlockParticles(ImpactPoint);
}

void ABaseCharacter::BallHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) // 파이어볼 맞는 함수
{
	if (IsAlive() && Hitter)
	{
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else Die();

	PlayArrowSound(ImpactPoint);
	SpawnArrowParticles(ImpactPoint);
}


void ABaseCharacter::GetStun_Implementation(const FVector& ImpactPoint, AActor* Hitter) // 몬스터가 스턴되는 함수
{
	if (IsAlive() && Hitter)
	{
		AttackReactParry();
	}
	else Die();

	PlayBlockSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

float ABaseCharacter::CalculateAngleBetweenPlayerAndMonster(AActor* Player, AActor* Hitter) //플레이어가 맞는 위치 계산 함수
{
	FVector PlayerLocation = Player->GetActorLocation();
	FVector MonsterLocation = Hitter->GetActorLocation();
	FVector DirectionToMonster = (MonsterLocation - PlayerLocation).GetSafeNormal();
	FVector PlayerForwardVector = Player->GetActorForwardVector();

	// 내적을 사용하여 두 벡터 사이의 각도를 계산
	float Angle = FMath::Acos(FVector::DotProduct(DirectionToMonster, PlayerForwardVector));
	Angle = FMath::RadiansToDegrees(Angle);

	return Angle;
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::Die()
{
	Tags.Add(FName("Dead"));
	PlayDeathMontage();
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::PlayStunReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && StunMontage)
	{
		AnimInstance->Montage_Play(StunMontage);
		AnimInstance->Montage_JumpToSection(SectionName, StunMontage);
	}
}

void ABaseCharacter::PlayFallReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FallReactMontage)
	{
		AnimInstance->Montage_Play(FallReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, FallReactMontage);
	}
}

void ABaseCharacter::PlayBlockReactMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && BlockMontage)
	{
	AnimInstance->Montage_Play(BlockMontage);
	}
}

void ABaseCharacter::DirectionalHitFall(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	// Lower Impact Point to the Enemy's Actor Location Z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();
	// Forward * ToHit = |Forward||ToHit| * cos(theta)
	// |Forward| = 1, |ToHit| = 1, so Forward * ToHit = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	// Take the inverse cosine (arc-cosine) of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta);
	// convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);
	// if CrossProduct points down, Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}
	FName Section("FromBack");
	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}
	PlayFallReactMontage(Section);
}

void ABaseCharacter::DirectionalHitStun(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	// Lower Impact Point to the Enemy's Actor Location Z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();
	// Forward * ToHit = |Forward||ToHit| * cos(theta)
	// |Forward| = 1, |ToHit| = 1, so Forward * ToHit = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	// Take the inverse cosine (arc-cosine) of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta);
	// convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);
	// if CrossProduct points down, Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}
	FName Section("FromBack");
	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}
	PlayStunReactMontage(Section);
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	// Lower Impact Point to the Enemy's Actor Location Z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();
	// Forward * ToHit = |Forward||ToHit| * cos(theta)
	// |Forward| = 1, |ToHit| = 1, so Forward * ToHit = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	// Take the inverse cosine (arc-cosine) of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta);
	// convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);
	// if CrossProduct points down, Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}
	FName Section("FromBack");
	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}
	PlayHitReactMontage(Section);
}

void ABaseCharacter::PlayArrowSound(const FVector& ImpactPoint)
{
	if (ArrowSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ArrowSound,
			ImpactPoint
		);
	}
}

void ABaseCharacter::PlayRespawnSound()
{
	if (RespawnSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			RespawnSound,
			GetActorLocation()
		);
	}
}

void ABaseCharacter::PlayBlessInteractSound()
{
	if (BlessInteractSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			BlessInteractSound,
			GetActorLocation()
		);
	}
}

void ABaseCharacter::SpawnArrowParticles(const FVector& ImpactPoint)
{
	if (ArrowParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ArrowParticles,
			ImpactPoint
		);
	}
}

void ABaseCharacter::PlayWeaponSkillSound()
{
	if (WeaponSkillSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			WeaponSkillSound,
			GetActorLocation()
		);
	}
}

void ABaseCharacter::PlayDamageIncreaseSound()
{
	if (DamageIncreaseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			DamageIncreaseSound,
			GetActorLocation()
		);
	}
}

void ABaseCharacter::PlayDamageIncreaseFailSound()
{
	if (DamageIncreaseFailSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			DamageIncreaseFailSound,
			GetActorLocation()
		);
	}
}

void ABaseCharacter::PlayRushSkillSound()
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

void ABaseCharacter::PlayAllMenuOpenSound()
{
	if (AllMenuOpenSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			AllMenuOpenSound,
			GetActorLocation()
		);
	}
}

void ABaseCharacter::PlayAllMenuCloseSound()
{
	if (AllMenuCloseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			AllMenuCloseSound,
			GetActorLocation()
		);
	}
}

void ABaseCharacter::PlayBlockSound(const FVector& ImpactPoint)
{
	if (BlockSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			BlockSound,
			ImpactPoint
		);
	}
}

void ABaseCharacter::SpawnBlockParticles(const FVector& ImpactPoint)
{
	if (BlockParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			BlockParticles,
			ImpactPoint
		);
	}
}
void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint
		);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles,
			ImpactPoint
		);
	}
}
void ABaseCharacter::SpawnEffect(const FVector& ImpactPoint)
{
	if (WeaponParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			WeaponParticles,
			ImpactPoint
		);
	}
}
FVector ABaseCharacter::GetWeaponTraceEndLocation() const
{
	// 궤적 길이 설정
	float TraceLength = 1000.0f; 

	// 무기 방향 벡터 계산
	FVector WeaponForwardVector = GetActorForwardVector();

	// 무기 궤적의 끝 위치 계산
	FVector TraceEndLocation = GetActorLocation() + WeaponForwardVector * TraceLength;

	return TraceEndLocation;
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
		Attributes->ReceiveStun(DamageAmount);
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}

	return Selection;
}

int32 ABaseCharacter::PlayStunMontage()
{
	return PlayRandomMontageSection(StunMontage, StunMontageSections);
}

void ABaseCharacter::PlayDiveMontage()
{
	PlayMontageSection(DiveMontage, FName("Dive"));
}

void ABaseCharacter::PlayDrinkMontage()
{
	PlayMontageSection(DrinkMontage, FName("Drink"));
}

void ABaseCharacter::PlayGetUpMontage()
{
	PlayMontageSection(GetUpMontage, FName("GetUp"));
}

void ABaseCharacter::PlayRespawnMontage()
{
	PlayMontageSection(RespawnMontage, FName("Respawn"));
}

void ABaseCharacter::PlayNeckSkillMontage()
{
	PlayMontageSection(NeckSkillMontage, FName("Skill"));
}

void ABaseCharacter::PlayWeaponSpellSkillMontage()
{
	PlayMontageSection(WeaponSpellSkillMontage, FName("WeaponSpellSkill"));
}

void ABaseCharacter::PlayLargeSkillMontage()
{
	PlayMontageSection(LargeSkillMontage, FName("LargeSkill"));
}

void ABaseCharacter::PlayGuardCounterMontage()
{
	PlayMontageSection(GuardCounterMontage, FName("GuardCounter"));
}

void ABaseCharacter::PlaySmallSkillMontage()
{
	PlayMontageSection(SmallSkillMontage, FName("SmallSkill"));
}

void ABaseCharacter::PlaySwordSkillMontage()
{
	PlayMontageSection(SwordSkillMontage, FName("Skill"));
}

void ABaseCharacter::PlayLaserSkillMontage()
{
	PlayMontageSection(LaserSkillMontage, FName("LaserSkill"));
}

void ABaseCharacter::PlayRushSkillMontage()
{
	PlayMontageSection(RushSkillMontage, FName("RushSkill"));
}

void ABaseCharacter::PlaySmashSkillMontage()
{
	PlayMontageSection(SmashSkillMontage, FName("SmashSkill"));
}

void ABaseCharacter::PlayMagicSkillMontage()
{
	PlayMontageSection(MagicSkillMontage, FName("MagicSkill"));
}

void ABaseCharacter::PlaySwingSkillMontage()
{
	PlayMontageSection(SwingSkillMontage, FName("SwingSkill"));
}

void ABaseCharacter::PlayTeleportSkillMontage()
{
	PlayMontageSection(TeleportSkillMontage, FName("TeleportSkill"));
}

void ABaseCharacter::PlayPhaseEnterMontage()
{
	PlayMontageSection(PhaseEnterMontage, FName("PhaseEnter"));
}


void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}

void ABaseCharacter::EnableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe *= WarpTargetDistance;

	return CombatTargetLocation + TargetToMe;

}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::EnableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

bool ABaseCharacter::CanLaserAttack()
{
	return false;
}

bool ABaseCharacter::CanRushAttack()
{
	return false;
}

bool ABaseCharacter::CanSwingAttack()
{
	return false;
}

bool ABaseCharacter::CanSmashAttack()
{
	return false;
}

bool ABaseCharacter::CanMagicAttack()
{
	return false;
}


bool ABaseCharacter::CanNeckSkill()
{
	return false;
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

bool ABaseCharacter::IsStun()
{
	return Attributes && Attributes->IsStun();
}

bool ABaseCharacter::NotEnoughStamina()
{
	return Attributes && Attributes->NotEnoughStamina();
}


bool ABaseCharacter::IsAttacking()
{
	return LichEnemy && LichEnemy->IsEngaged();
}

bool ABaseCharacter::HasEnoughShieldStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetShieldCost();
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::BlockEnd()
{
}

void ABaseCharacter::HitEnd()
{
}

void ABaseCharacter::StunEnd()
{
}

void ABaseCharacter::StunStart()
{
}

void ABaseCharacter::DiveEnd()
{
}

void ABaseCharacter::EndLaserSkill()
{
}

void ABaseCharacter::EndRushSkill()
{
}

void ABaseCharacter::EndSmashSkill()
{
}

void ABaseCharacter::EndMagicSkill()
{
}

void ABaseCharacter::EndSwingSkill()
{
}

void ABaseCharacter::EndEnterPhase()
{
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void ABaseCharacter::SetSkillCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetSkillBox())
	{
		EquippedWeapon->GetSkillBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void ABaseCharacter::SetRushSkillCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetRushSkillBox())
	{
		EquippedWeapon->GetRushSkillBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void ABaseCharacter::SetClawSkillCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetClawSkillBox())
	{
		EquippedWeapon->GetClawSkillBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}
void ABaseCharacter::SetTeethSkillCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetTeethSkillBox())
	{
		EquippedWeapon->GetTeethSkillBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}
void ABaseCharacter::SetShieldCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedShield && EquippedShield->GetShieldBox())
	{
		EquippedShield->GetShieldBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedShield->IgnoreActors.Empty();
	}
}
void ABaseCharacter::SetLeftCastSkillCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetLeftCastSkillBox())
	{
		EquippedWeapon->GetLeftCastSkillBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}




