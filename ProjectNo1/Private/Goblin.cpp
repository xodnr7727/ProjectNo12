// Fill out your copyright notice in the Description page of Project Settings.


#include "Goblin.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Weapons/Weapon.h"
#include "Weapons/Shield.h"
#include "Weapons/TopArmor.h"
#include "Weapons/EnemyWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Soul.h"
#include "Items/Treasure.h"
#include "HUD/SlashOverlay.h"

// Sets default values
AGoblin::AGoblin()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

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

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Script /Engine.SkeletalMesh'/Game/Goblin/Mesh/Goblin_armor_ice/SK_Goblin_Body_Armor_ice.SK_Goblin_Body_Armor_ice'"));

	if (MeshAsset.Succeeded())
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);

}
void AGoblin::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &AGoblin::PawnSeen);
	InitializeEnemy();
	SetInitialSpawnLocation();
	Tags.Add(FName("Enemy"));

	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

}

void AGoblin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHitNumbers();
	if (IsDead()) return;
	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}


void AGoblin::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AGoblin::PatrolTimerFinished, WaitTime);
	}
}

void AGoblin::CheckCombatTarget()
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

void AGoblin::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)//타격 받는 함수
{
	if (IsEngaged()) return;

	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if (!IsDead()) ShowHealthBar();

	ClearPatrolTimer();
	ClearAttackTimer();
	GetCharacterMovement()->Activate();
	EnemyState = EEnemyState::EES_Attacking;
	StopAttackMontage();

	if (IsInsideAttackRadius())
	{
		if (!IsDead()) StartAttackTimer();
	}
}

void AGoblin::TakeExecutionHold()
{
		// 몬스터 움직임 고정
		GetCharacterMovement()->DisableMovement();
}

void AGoblin::TakeBack()
{
	// 다시 몬스터 움직임 활성화
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void AGoblin::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("PointSword"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}
void AGoblin::SpawnDefaultWeaponTwo()
{
	UWorld* World = GetWorld();
	if (World && ShieldClass)
	{
		AShield* DefaultShield = World->SpawnActor<AShield>(ShieldClass);
		DefaultShield->Equip(GetMesh(), FName("LeftWeapon"), this, this);
		EquippedShield = DefaultShield;
	}
}

void AGoblin::SpawnDefaultAmor()
{
	UWorld* World = GetWorld();
	if (World && AmorClass)
	{
		ATopArmor* DefaultArmor = World->SpawnActor<ATopArmor>(AmorClass);
		DefaultArmor->Equip(GetMesh(), FName("PointArmor"), this, this);
		EquippedArmor = DefaultArmor;
	}
}

void AGoblin::Die()
{
	FTimerHandle RespawnTimerHandle;
	Super::Die();
	EnemyState = EEnemyState::EES_Dead;
	bGoblinDead = true;
	ClearAttackTimer();
	HideHealthBar();
	DisableCapsule();
	GetCharacterMovement()->Deactivate();
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetActorEnableCollision(false);
	SpawnEx();
	SpawnGd();
}

void AGoblin::DieState()
{
	EnemyState = EEnemyState::EES_Dead;
	bGoblinDead = true;
}

void AGoblin::Respawn()
{
	EnemyController = Cast<AAIController>(GetController());
	PlayRespawnMontage();
	RestartMonsterAtLocation(RespawnLocation);
	EnemyState = EEnemyState::EES_NoState;
	MoveToTarget(PatrolTarget);
	bGoblinDead = false;
	EnableCapsule();
	GetCharacterMovement()->Activate();
	GetCharacterMovement()->bOrientRotationToMovement = true;
	SetActorEnableCollision(true);
	Attributes->Health += Attributes->MaxHealth;
}

void AGoblin::RestartMonsterAtLocation(FVector RestartspawnLocation)
{
	SetActorLocation(RespawnLocation); // 리스폰 위치로 이동
	SetActorRotation(FRotator::ZeroRotator); // 기본 회전으로 설정
}

void AGoblin::SetInitialSpawnLocation()
{
	RespawnLocation = GetActorLocation();
}

void AGoblin::SpawnEx()
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
void AGoblin::SpawnGd()
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

bool AGoblin::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AGoblin::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(50.f);
	EnemyController->MoveTo(MoveRequest);
}

float AGoblin::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) 
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

void AGoblin::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
	if (EquippedShield)
	{
		EquippedShield->Destroy();
	}

	OnDestroyedDetected.Broadcast();
}

void AGoblin::StoreHitNumber(UUserWidget* HitNubmer, FVector Location)
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

void AGoblin::DestroyHitNumber(UUserWidget* HitNumber)
{
	HitNumbers.Remove(HitNumber);
	HitNumber->RemoveFromParent();
}

void AGoblin::UpdateHitNumbers()
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

void AGoblin::PatrolTimerFinished()
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
}

void AGoblin::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName("EngageableTarget"));

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}

AActor* AGoblin::ChoosePatrolTarget()
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

void AGoblin::AttackSweepTrace()
{
	FVector Start = GetActorLocation() + GetActorForwardVector() * 100.0f; // 보스 바로 앞 위치에서 시작
	FVector ForwardVector = GetActorForwardVector(); // 몬스터가 향하는 방향
	FVector End = Start + ForwardVector * 100.0f; // 일정 거리만큼 스윕 트레이스
	FVector SpawnLocation = GetActorLocation();
	//FHitResult HitResult;
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // 보스는 무시
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECollisionChannel::ECC_Visibility,
		FCollisionShape::MakeSphere(50.0f),
		CollisionParams);
	UE_LOG(LogTemp, Log, TEXT("GoblinAttackSweepTrace")); //확인완료
	// 라인 트레이스의 시작점에 이펙트를 생성하여 표시

	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EndPointEffect, SpawnLocation, FRotator::ZeroRotator);
	// 라인 트레이스 실행
	if (bResult) {
		if (AActor* Actor = HitResult.GetActor()) {
			if (HitResult.GetActor()->ActorHasTag("EngageableTarget"))
			{
				UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *HitResult.GetActor()->GetName());
				//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.f, 0, 1.f);
				// 라인 트레이스의 타격점에 이펙트를 생성하여 표시
				//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, HitResult.ImpactPoint, FRotator::ZeroRotator);
				//PlayWeaponSpellHitSound(HitResult.ImpactPoint);
				FDamageEvent DamageEvent;

				UGameplayStatics::ApplyDamage(HitResult.GetActor(), GoblinDamage, GetInstigatorController(), this, UDamageType::StaticClass());
				ExecuteGetHit(HitResult);
				ExecuteGetBlock(HitResult);
			}
		}
	}
	else {
		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.f, 0, 1.f);
	}
}

void AGoblin::ExecuteGetHit(FHitResult& HitResult)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(HitResult.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetHit(HitResult.GetActor(), HitResult.ImpactPoint, GetOwner());
	}
}

void AGoblin::ExecuteGetBlock(FHitResult& HitResult)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(HitResult.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetBlock(HitResult.GetActor(), HitResult.ImpactPoint, GetOwner());
	}
}

void AGoblin::Attack()
{
	Super::Attack();
	if (CombatTarget == nullptr || IsDead()) return;
	GetCharacterMovement()->Deactivate();
	EnemyState = EEnemyState::EES_Engaged;
	PlayAttackMontage();
}

bool AGoblin::CanAttack()
{
	bool bCanAttack =
		IsInsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsDead();
	return bCanAttack;
}

void AGoblin::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	GetCharacterMovement()->Activate();
	CheckCombatTarget();
}

void AGoblin::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AGoblin::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	HideHealthBar();
	SpawnDefaultWeapon();
	SpawnDefaultWeaponTwo();
}

void AGoblin::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AGoblin::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

void AGoblin::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AGoblin::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AGoblin::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

void AGoblin::CombatTargetPlayer()
{
	EnemyState = EEnemyState::EES_Chasing;
	UE_LOG(LogTemp, Log, TEXT("PlayerCombat"));
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(ProjectNo1Character);
}

bool AGoblin::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AGoblin::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AGoblin::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AGoblin::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AGoblin::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AGoblin::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead ||
		bGoblinDead == true;
}

bool AGoblin::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AGoblin::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AGoblin::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AGoblin::Attack, AttackTime);
}

void AGoblin::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

// 발사체 액터 생성 함수
void AGoblin::ProjectileAttack()
{
	// 화살이 발사될 위치를 설정한다
	FVector vPos = GetActorLocation() + GetActorUpVector() * 30.f + GetActorForwardVector() * 100.f;

	// 스폰 파라미터를 설정한다
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 월드에 액터를 배치한다
	AProjectileWeapon* ProjectileWeapon = GetWorld()->SpawnActor<AProjectileWeapon>(ProjectileWeaponClass, vPos, GetActorRotation(), params);
}
