// Fill out your copyright notice in the Description page of Project Settings.


#include "BossCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "HUD/StunBarComponent.h"
#include "Weapons/Weapon.h"
#include "Weapons/EnemyWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h"
#include "Soul.h"
#include "Items/Treasure.h"
#include "HUD/SlashOverlay.h"

ABossCharacter::ABossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bAttack = true;

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	StunBarWidget = CreateDefaultSubobject<UStunBarComponent>(TEXT("StunBar"));
	StunBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);
	HitNumberDestroyTime = 1.5f;

	static ConstructorHelpers::FClassFinder<AProjectileWeapon> ProjectileAsset(TEXT("Blueprint'/Game/ThirdPerson/Blueprints/Enemy/Weapon/EnemyProjectileWeapon'"));

	if (ProjectileAsset.Succeeded())
		ProjectileWeaponClass = ProjectileAsset.Class;

}
void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &ABossCharacter::PawnSeen);
	InitializeEnemy();
	Tags.Add(FName("Enemy"));

}

void ABossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHitNumbers();
	if (IsDead()) return;
	if (Attributes && HealthBarWidget)
	{
		Attributes->RegenHealth(DeltaTime);
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
	if (PlayerCharacter)
	{
		// 플레이어와의 거리 계산
		FVector PlayerLocation = PlayerCharacter->GetActorLocation();
		FVector MonsterLocation = GetActorLocation();
		float DistanceToPlayer = FVector::Distance(PlayerLocation, MonsterLocation);

		// 플레이어에게 가까우면 이동하고 공격
		// or 플레이어가 일정 범위내에 들어오면 원거리 공격
		if (DistanceToPlayer < RushRadius)
		{
			// 플레이어를 향해 이동
			FVector DirectionToPlayer = PlayerLocation - MonsterLocation;
			DirectionToPlayer.Normalize();
			AddMovementInput(DirectionToPlayer, ChasingSpeed * DeltaTime);

			// 공격 로직을 여기에 추가 (예: 타이머를 사용하여 주기적으로 공격)
		}
	}
}

void ABossCharacter::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &ABossCharacter::PatrolTimerFinished, WaitTime);
	}
}

void ABossCharacter::CheckCombatTarget()
{
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if (!IsEngaged()) StartPatrolling();
	}
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		// Outside attack range, chase character
		EnemyState = EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		MoveToTarget(CombatTarget);
	}
	else if (CanAttack())
	{
		StartAttackTimer();
	}
}

void ABossCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)//타격 받는 함수
{
	if (IsEngaged()) return;

	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if (!IsDead()) {
		ShowHealthBar();
		ShowStunBar();
	}
	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	EnemyState = EEnemyState::EES_Attacking;
	StopAttackMontage();

	if (IsInsideAttackRadius())
	{
		if (!IsDead()) StartAttackTimer();
	}
}

void ABossCharacter::BallHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsEngaged()) return;

	Super::BallHit_Implementation(ImpactPoint, Hitter);
	if (!IsDead()) {
		ShowHealthBar();
		ShowStunBar();
	}

	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	EnemyState = EEnemyState::EES_Attacking;
	StopAttackMontage();
}

void ABossCharacter::GetStun_Implementation(const FVector& ImpactPoint, AActor* Hitter)//막는 함수
{
	if (!IsEngaged()) return;

	Super::GetStun_Implementation(ImpactPoint, Hitter);
	if (!IsDead()) ShowHealthBar();

	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	EnemyState = EEnemyState::EES_Stunned;
	StopAttackMontage();
}

void ABossCharacter::RecoveryStunState() {
	if (Attributes && StunBarWidget)
	{
		Attributes->RecoveryStun();
		StunBarWidget->SetStunPercent(Attributes->GetStunPercent());
	}
}

void ABossCharacter::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("MouthWeapon"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void ABossCharacter::SpawnLeftWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("LeftHandWeapon"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void ABossCharacter::SpawnRightWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("RightHandWeapon"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}


void ABossCharacter::Die()
{
	EnemyState = EEnemyState::EES_Dead;
	PlayDeathMontage();
	ClearAttackTimer();
	HideHealthBar();
	HideStunBar();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorEnableCollision(false);
	SpawnEx();
	SpawnGd();
}

void ABossCharacter::Stun()
{
	EnemyState = EEnemyState::EES_Stunned;
	PlayStunMontage();
	ClearAttackTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABossCharacter::SpawnEx()
{
	UWorld* World = GetWorld();
	if (World && ExClass && Attributes)
	{
		ASoul* SpawnedEx = World->SpawnActor<ASoul>(ExClass, GetActorLocation(), GetActorRotation());
		if (SpawnedEx)
		{
			SpawnedEx->SetSouls(Attributes->GetSouls());
		}
	}
}
void ABossCharacter::SpawnGd()
{
	UWorld* World = GetWorld();
	if (World && GdClass && Attributes)
	{
		ATreasure* SpawnedGd = World->SpawnActor<ATreasure>(GdClass, GetActorLocation(), GetActorRotation());
		if (SpawnedGd)
		{
			SpawnedGd->SetGold(Attributes->GetGold());
		}
	}
}

bool ABossCharacter::InTargetRange(AActor* Target, double Radius) //플레이어가 범위 내
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void ABossCharacter::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(50.f);
	EnemyController->MoveTo(MoveRequest);
}

float ABossCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) //������ �ޱ�
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();

	if (IsInsideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (IsOutsideAttackRadius())
	{
		ChaseTarget();
	}
	return DamageAmount;
}

void ABossCharacter::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void ABossCharacter::StoreHitNumber(UUserWidget* HitNubmer, FVector Location)
{
	HitNumbers.Add(HitNubmer, Location);

	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;
	HitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"), HitNubmer);
	GetWorld()->GetTimerManager().SetTimer(
		HitNumberTimer,
		HitNumberDelegate,
		HitNumberDestroyTime,
		false);
}

void ABossCharacter::DestroyHitNumber(UUserWidget* HitNumber)
{
	HitNumbers.Remove(HitNumber);
	HitNumber->RemoveFromParent();
}

void ABossCharacter::UpdateHitNumbers()
{
	for (auto& HitPair : HitNumbers) {
		UUserWidget* HitNumber{ HitPair.Key };
		const FVector Location{ HitPair.Value };
		FVector2D ScreenPosition;

		UGameplayStatics::ProjectWorldToScreen(
			GetWorld()->GetFirstPlayerController(),
			Location,
			ScreenPosition);
		HitNumber->SetPositionInViewport(ScreenPosition);
	}
}

void ABossCharacter::PatrolTimerFinished()
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
}

void ABossCharacter::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState != EEnemyState::EES_Stunned &&
		EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName("EngageableTarget"));

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}

AActor* ABossCharacter::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}
	return nullptr;
}

void ABossCharacter::Attack() //공격 함수
{
	Super::Attack();
	if (CombatTarget == nullptr) return;

	if (bAttack == false) return;
	EnemyState = EEnemyState::EES_Engaged;
	PlayAttackMontage();
}

bool ABossCharacter::CanAttack()
{
	bool bCanAttack =
		IsInsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsStunned() &&
		!IsDead();
	return bCanAttack;
}

void ABossCharacter::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void ABossCharacter::HitEnd()
{
	EnemyState = EEnemyState::EES_Attacking;
	GetCharacterMovement()->Activate();//이동 허용
	bAttack = true;
}

void ABossCharacter::StunEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	GetCharacterMovement()->Activate();//이동 허용
	bAttack = true;	// 공격 가능 상태로 전환
}

void ABossCharacter::StunStart()
{
	EnemyState = EEnemyState::EES_Stunned;
	GetCharacterMovement()->Deactivate(); //이동 억제
	bAttack = false; 	// 공격 불가능 상태로 전환
}

void ABossCharacter::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
		if (Attributes && StunBarWidget)
		{
			StunBarWidget->SetStunPercent(Attributes->GetStunPercent());
		}
}

int32 ABossCharacter::PlayDeathMontage()
{
	const int32 Selection = Super::PlayDeathMontage();
	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}
	return Selection;
}

void ABossCharacter::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	HideHealthBar();
	HideStunBar();
	SpawnDefaultWeapon();
	SpawnLeftWeapon();
	SpawnRightWeapon();
}

void ABossCharacter::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void ABossCharacter::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}
void ABossCharacter::HideStunBar()
{
	if (StunBarWidget)
	{
		StunBarWidget->SetVisibility(false);
	}
}

void ABossCharacter::ShowStunBar()
{
	if (StunBarWidget)
	{
		StunBarWidget->SetVisibility(true);
	}
}
void ABossCharacter::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
	HideStunBar();
}

void ABossCharacter::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void ABossCharacter::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

bool ABossCharacter::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool ABossCharacter::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool ABossCharacter::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool ABossCharacter::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool ABossCharacter::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool ABossCharacter::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool ABossCharacter::Alive()
{
	return Attributes && Attributes->IsAlive();
}

bool ABossCharacter::IsStunned()
{
	return EnemyState == EEnemyState::EES_Stunned;
}

bool ABossCharacter::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void ABossCharacter::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void ABossCharacter::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &ABossCharacter::Attack, AttackTime);
}

void ABossCharacter::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

// 발사체 액터 생성 함수
void ABossCharacter::ProjectileAttack()
{
	// 화살이 발사될 위치를 설정한다
	FVector vPos = GetActorLocation() + GetActorUpVector() * 30.f + GetActorForwardVector() * 100.f;

	// 스폰 파라미터를 설정한다
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 월드에 액터를 배치한다
	AProjectileWeapon* ProjectileWeapon = GetWorld()->SpawnActor<AProjectileWeapon>(ProjectileWeaponClass, vPos, GetActorRotation(), params);
}
