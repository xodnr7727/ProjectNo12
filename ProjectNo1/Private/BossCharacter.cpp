// Fill out your copyright notice in the Description page of Project Settings.


#include "BossCharacter.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "HUD/StunBarComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Weapons/Weapon.h"
#include "Weapons/EnemyWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h"
#include "Engine/World.h"
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

	LaserSkillCooldown = 20.0f; // 초기 쿨타임 설정 (예: 20초)
	bCanLaserSkill = true; //초기에 레이저 공격 사용할 수 있도록 설정
	LaserSkillTimer = 10.0f; // 초기 지속시간 설정 (예: 10초)

	// Set the default value for the special targeting range
	LaserSkillEnableRange = 800.0f;

	RushSkillCooldown = 15.0f; // 초기 쿨타임 설정 (예: 20초)
	bCanRushSkill = true; //초기에 돌진 공격 사용할 수 있도록 설정

	AllSkillCooldown = 7.0f; // 스킬 쿨타임 설정 (예: 7초)
	bCanSkill = true;  //초기에 스킬 사용할 수 있도록 설정
	CollisonTimer = 1.0f; // 스킬 쿨타임 설정 (예: 1초)

	// Set the default value for the special targeting range
	RushSkillEnableRange = 300.0f;

	RushSkillEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RushSkillEffect"));
	RushSkillEffect->SetupAttachment(RootComponent); // 이펙트 위치 설정

	LaserSkillEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LaserSkillEffect"));
	LaserSkillEffect->SetupAttachment(RootComponent); // 이펙트 위치 설정

}


void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();
	RushSkillEffect->DeactivateSystem(); // 초기에는 비활성화
	LaserSkillEffect->DeactivateSystem(); // 초기에는 비활성화

	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &ABossCharacter::PawnSeen);
	InitializeEnemy();
	Tags.Add(FName("Enemy"));

	if (HasExistPlayerInFront())//일정 거리 멀리에 플레이어가 있다면
	{
		//UE_LOG(LogTemp, Log, TEXT("PrepareOnLaserSkill")); //확인완료
		OnLaserSkill();//스킬 공격
	}
	if (HasExistRushPlayerInFront())//일정 거리에 플레이어가 있다면
	{
		//UE_LOG(LogTemp, Log, TEXT("PrepareRushSkill")); //확인완료
		RushSkill();//스킬 공격
	}
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
	if (HasExistPlayerInFront())//일정 거리 멀리에 플레이어가 있다면
	{
		//UE_LOG(LogTemp, Log, TEXT("PrepareOnLaserSkill")); //확인완료
		OnLaserSkill();//스킬 공격
	}
	if (HasExistRushPlayerInFront())//일정 거리에 플레이어가 있다면
	{
		//UE_LOG(LogTemp, Log, TEXT("PrepareRushSkill")); //확인완료
		RushSkill();//스킬 공격
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

bool ABossCharacter::HasExistPlayerInFront()//플레이어 체크 함수
{
	TArray<FHitResult> HitResults;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * LaserSkillEnableRange;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // Ignore the boss
	CollisionParams.bTraceComplex = true;
	bool bHit = GetWorld()->SweepMultiByChannel(HitResults, StartLocation, EndLocation, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(50.0f), CollisionParams);

	if (bHit)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			AActor* OverlappingActor = HitResult.GetActor();
			// 액터의 클래스가 AProjectNo1Character인지 확인
			if (OverlappingActor->IsA(AProjectNo1Character::StaticClass()))
			{
				ProjectNo1Character = Cast<AProjectNo1Character>(OverlappingActor);

				if (ProjectNo1Character)
				{
					//UE_LOG(LogTemp, Log, TEXT("ProjectNo1CharacterCheck")); //확인완료
					return true;
				}
			}
		}
	}
	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 2.0f); //확인완료
	return false;
}

bool ABossCharacter::HasExistRushPlayerInFront()//플레이어 체크 함수
{
	TArray<FHitResult> HitResults;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * RushSkillEnableRange;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // Ignore the boss
	CollisionParams.bTraceComplex = true;
	bool bHit = GetWorld()->SweepMultiByChannel(HitResults, StartLocation, EndLocation, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(50.0f), CollisionParams);

	if (bHit)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			AActor* OverlappingActor = HitResult.GetActor();
			// 액터의 클래스가 AProjectNo1Character인지 확인
			if (OverlappingActor->IsA(AProjectNo1Character::StaticClass()))
			{
				ProjectNo1Character = Cast<AProjectNo1Character>(OverlappingActor);

				if (ProjectNo1Character)
				{
					//UE_LOG(LogTemp, Log, TEXT("ProjectNo1CharacterCheck")); //확인완료
					return true;
				}
			}
		}
	}
	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 2.0f); //확인완료
	return false;
}


void ABossCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)//타격 받는 함수
{
	if (IsEngaged()) return;

	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if (!IsDead()) {
		ShowHealthBar();
		ShowStunBar();
	}
	if (!IsEngaged()) {
		ClearPatrolTimer();
		ClearAttackTimer();
	}
	GetCharacterMovement()->Activate();
	//EnemyState = EEnemyState::EES_Attacking;
	//StopAttackMontage();
	bAttack = true;
	if (IsInsideAttackRadius())
	{
		if (!IsDead()) StartAttackTimer();
	}
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		// Outside attack range, chase character
		EnemyState = EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		MoveToTarget(CombatTarget);
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

void ABossCharacter::OnLaserSkill()
{
	FTimerHandle LaserSkillCountdown;
	FTimerHandle AllSkillCountdown;
	FTimerHandle CollisonCountTimer;

	if (CanLaserAttack() && bCanLaserSkill && bCanSkill)
	{
		PlayLaserSkillMontage();
		EnemyState = EEnemyState::EES_Engaged;
		bCanLaserSkill = false;//연속 사용 X
		bAttack = false;
		IncreaseDamage();// 공격력 증가
		GetCharacterMovement()->Deactivate();
		GetWorldTimerManager().SetTimer(LaserSkillCountdown, this, &ABossCharacter::EnableLaserSkill, LaserSkillCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(AllSkillCountdown, this, &ABossCharacter::EnableSkill, AllSkillCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(CollisonCountTimer, this, &ABossCharacter::EndLaserSkill, LaserSkillTimer, false); // 타이머 시작
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//레이저 버프 시전 시 무적
		PlayWeaponSkillSound(); //스킬 사운드 재생
	}
}

void ABossCharacter::IncreaseDamage()
{
	// 공격력을 증가시키는 코드
	BossDamage += 20.0f;
}

void ABossCharacter::RestoreDamage()
{
	// 공격력을 원래대로 복구시키는 코드
	// 예를 들어, CharacterDamage = BaseDamage;
	BossDamage -= 20.0f;
}

void ABossCharacter::EndLaserSkill()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제
	LaserSkillEffect->DeactivateSystem();// 레이저 버프 이펙트 비활성화
	RestoreDamage(); // 공격력 복구
}

void ABossCharacter::EnableLaserSkill()
{
	bCanLaserSkill = true;// 레이저 버프 사용 활성화
}

void ABossCharacter::ActivateLaserSkillEffect()
{
	LaserSkillEffect->ActivateSystem();
}

void ABossCharacter::RushSkill()
{
	FTimerHandle RushSkillCountdown;
	FTimerHandle SkillCountdown;
	FTimerHandle CollisonCountdown;

	if (CanRushAttack() && bCanRushSkill && bCanSkill) //스킬 우선순위 설정
	{
		PlayRushSkillMontage();
		EnemyState = EEnemyState::EES_Engaged;
		bCanRushSkill = false;//돌진 연속 사용 X
		bCanSkill = false;//스킬 연속 사용 X
		bAttack = false;
		GetCharacterMovement()->Deactivate();
		GetWorldTimerManager().SetTimer(RushSkillCountdown, this, &ABossCharacter::EnableRushSkill, RushSkillCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(SkillCountdown, this, &ABossCharacter::EnableSkill, AllSkillCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(CollisonCountdown, this, &ABossCharacter::EndRushSkill, CollisonTimer, false); // 쿨타임 타이머 시작
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//돌진 공격 시 무적
		PlayRushSkillSound(); //스킬 사운드 재생
	}
}

void ABossCharacter::RushSpellSweepTrace()
{
	FVector Start = GetActorLocation() + GetActorForwardVector() * 100.0f; // 보스 바로 앞 위치에서 시작
	FVector ForwardVector = GetActorForwardVector(); // 보스가 향하는 방향
	FVector End = Start + ForwardVector * 150.0f; // 일정 거리만큼 스윕 트레이스
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
		FCollisionShape::MakeSphere(80.0f),
		CollisionParams);
	UE_LOG(LogTemp, Log, TEXT("RushSpellSweepTrace")); //확인완료
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

				UGameplayStatics::ApplyDamage(HitResult.GetActor(), BossDamage, GetInstigatorController(), this, UDamageType::StaticClass());
				ExecuteGetHit(HitResult);
				ExecuteGetBlock(HitResult);
			}
		}
	}
	else {
		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.f, 0, 1.f);
	}
}
void ABossCharacter::ExecuteGetHit(FHitResult& HitResult)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(HitResult.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetHit(HitResult.GetActor(), HitResult.ImpactPoint, GetOwner());
	}
}

void ABossCharacter::ExecuteGetBlock(FHitResult& HitResult)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(HitResult.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetBlock(HitResult.GetActor(), HitResult.ImpactPoint, GetOwner());
	}
}

void ABossCharacter::EndRushSkill()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제
}

void ABossCharacter::EnableRushSkill()
{
	bCanRushSkill = true;// 돌진 공격 사용 활성화
}

void ABossCharacter::EnableSkill()
{
	bCanSkill = true;// 스킬 사용 활성화
}

void ABossCharacter::DisableRushSkill()
{
	bCanRushSkill = false;// 돌진 공격 사용 비활성화
}

void ABossCharacter::DeactivateRushSkillEffect()
{
	RushSkillEffect->DeactivateSystem(); // 돌진 이펙트 비활성화
}

void ABossCharacter::ActivateRushSkillEffect()
{
	RushSkillEffect->ActivateSystem(); // 돌진 이펙트 활성화
}

void ABossCharacter::Die()
{
	Super::Die();
	EnemyState = EEnemyState::EES_Dead;
	ClearAttackTimer();
	HideHealthBar();
	HideStunBar();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetActorEnableCollision(false);
	SpawnEx();
	SpawnGd();
}

void ABossCharacter::Stun()
{
	EnemyState = EEnemyState::EES_Stunned;
	PlayStunMontage();
	ClearAttackTimer();
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
	FTimerHandle AttackCollisonCountdown;
	Super::Attack();
	if (CanAttack()) return;
	if (CombatTarget == nullptr) return;
	if (bAttack == false) return;
	GetCharacterMovement()->Deactivate();
	bAttack = false;
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

bool ABossCharacter::CanLaserAttack()
{
	bool bCanLaserAttack =
		IsOutsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsStunned() &&
		!IsDead();
	return bCanLaserAttack;
}

bool ABossCharacter::CanRushAttack()
{
	bool bCanRushAttack =
		IsOutsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsStunned() &&
		!IsDead();
	return bCanRushAttack;
}

void ABossCharacter::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	GetCharacterMovement()->Activate();
	CheckCombatTarget();
	bAttack = true;
}


void ABossCharacter::HitEnd()
{
	EnemyState = EEnemyState::EES_NoState;
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

	if (Attributes && HealthBarWidget)//데미지 적용
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
		if (Attributes && StunBarWidget)//무력화 데미지 적용
		{
			StunBarWidget->SetStunPercent(Attributes->GetStunPercent());
		}
}

void ABossCharacter::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	HideHealthBar();
	HideStunBar();
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

void ABossCharacter::CombatTargetPlayer()
{
	EnemyState = EEnemyState::EES_Chasing;
	UE_LOG(LogTemp, Log, TEXT("PlayerCombat"));
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(ProjectNo1Character);
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

bool ABossCharacter::IsCanLaserSkill()
{
	return Attributes && Attributes->IsCanLaserSkill();
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
