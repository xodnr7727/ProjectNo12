// Fill out your copyright notice in the Description page of Project Settings.


#include "LichEnemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Weapons/Weapon.h"
#include "Weapons/EnemyWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "AIController.h"
#include "Weapons/Shield.h"
#include "Soul.h"
#include "Items/Treasure.h"
#include "HUD/SlashOverlay.h"
#include <NiagaraFunctionLibrary.h>

ALichEnemy::ALichEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	bAttack = true;
	bAction = false;

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
	LeftCastSkillCount = 0.1f;

	static ConstructorHelpers::FClassFinder<AProjectileWeapon> ProjectileAsset(TEXT("Blueprint'/Game/ThirdPerson/Blueprints/Enemy/Weapon/EnemyProjectileWeapon'"));

	if (ProjectileAsset.Succeeded())
		ProjectileWeaponClass = ProjectileAsset.Class;

	SmashSkillCooldown = 21.0f; // 초기 쿨타임 설정 (예: 21초)
	bCanSmashSkill = true; //초기에 레이저 공격 사용할 수 있도록 설정

	// Set the default value for the special targeting range
	SmashSkillEnableRange = 500.0f;

	MagicSkillCooldown = 8.0f; // 초기 쿨타임 설정 (예: 8초)
	bCanMagicSkill = true; //초기에 레이저 공격 사용할 수 있도록 설정

	// Set the default value for the special targeting range
	MagicSkillEnableRange = 300.0f;

	SwingSkillCooldown = 17.0f; // 초기 쿨타임 설정 (예: 17초)
	bCanSwingSkill = true; //초기에 돌진 공격 사용할 수 있도록 설정

	// Set the default value for the special targeting range
	SwingSkillEnableRange = 300.0f;

	TeleportSkillCooldown = 13.0f; // 텔레포트 스킬 쿨타임 변수
	bCanTeleportSkill = true;

	TeleportSkillEnableRange = 700.0f;
	TeleportDistance = 30.0f;

	AllSkillCooldown = 4.0f; // 스킬 쿨타임 설정 (예: 4초)
	bCanSkill = true;  //초기에 스킬 사용할 수 있도록 설정
	CollisonTimer = 1.0f; // 무적 쿨타임 설정 (예: 1초)

	bIsPhaseTwo = false;
	PhaseTwoTimer = 7.0f;
}
void ALichEnemy::BeginPlay()
{
	Super::BeginPlay();

		if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &ALichEnemy::PawnSeen);
		InitializeEnemy();
		Tags.Add(FName("Enemy"));
		InitialLocation = GetActorLocation();

		OnSwingSkillDetected.AddDynamic(this, &ALichEnemy::SwingSkill);
		OnSmashSkillDetected.AddDynamic(this, &ALichEnemy::SmashSkill);
		OnMagicSkillDetected.AddDynamic(this, &ALichEnemy::MagicSkill);
		OnTeleportSkillDetected.AddDynamic(this, &ALichEnemy::TeleportSkill);
		OnBossPhaseDelegated.AddDynamic(this, &ALichEnemy::EnterPhaseTwo);
}

void ALichEnemy::Tick(float DeltaTime)
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
	CheckForSwing();
	CheckForSmash();
	CheckForMagic();
	CheckForTeleport();
	CheckHealth();
}

void ALichEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &ALichEnemy::PatrolTimerFinished, WaitTime);
	}
}

void ALichEnemy::CheckCombatTarget()
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

bool ALichEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void ALichEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(50.f);
	EnemyController->MoveTo(MoveRequest);
}

void ALichEnemy::CheckForSwing()
{
	if (HasExistPlayerInFront())
	{
		// 델리게이트 호출
		OnSwingSkillDetected.Broadcast();
	}
}

void ALichEnemy::CheckForSmash()
{
	if (HasExistSmashPlayerInFront())
	{
		// 델리게이트 호출	
		OnSmashSkillDetected.Broadcast();
	}
}

void ALichEnemy::CheckForMagic()
{
	if (HasExistMagicPlayerInFront())
	{
		// 델리게이트 호출	
		OnMagicSkillDetected.Broadcast();
	}
}

void ALichEnemy::CheckForTeleport()
{
	if (HasExistTeleportPlayerInFront())
	{
		// 델리게이트 호출
		OnTeleportSkillDetected.Broadcast();
	}
}

void ALichEnemy::CheckHealth()
{
	if (Attributes->Health <=  Attributes->MaxHealth / 2 && !bIsPhaseTwo)
	{
		OnBossPhaseDelegated.Broadcast();
	}
}

void ALichEnemy::EnterPhaseTwo()
{
	FTimerHandle PhaseTwoCountdown;

	bIsPhaseTwo = true;
	bAttack = false; //기본 공격 변수
	bAction = true; //행동 변수
	EnemyState = EEnemyState::EES_Engaged;
	UE_LOG(LogTemp, Log, TEXT("EnterPhaseTwo")); 
	PlayPhaseEnterMontage();
	GetCharacterMovement()->Deactivate();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetWorldTimerManager().SetTimer(PhaseTwoCountdown, this, &ALichEnemy::EnterEndPhaseTwo, PhaseTwoTimer, false); // 타이머 시작
}

void ALichEnemy::ResetPositions()
{
	// 보스 위치 초기화
	SetActorLocation(InitialLocation);

}

void ALichEnemy::EnterEndPhaseTwo()
{
	EnemyState = EEnemyState::EES_NoState;
	ResetPositions();
	ChangeBossAppearance();
	GetCharacterMovement()->Activate();
	CheckCombatTarget();
	bAttack = true; //기본 공격 변수
	bAction = false; //행동 변수
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ALichEnemy::ChangeBossAppearance()
{
	if (PhaseTwoMesh)
	{
		USkeletalMeshComponent* MeshComponent = FindComponentByClass<USkeletalMeshComponent>();
		if (MeshComponent)
		{
			MeshComponent->SetSkeletalMesh(PhaseTwoMesh);
			UE_LOG(LogTemp, Log, TEXT("ChangeBossAppearance"));
		}
	}
}

void ALichEnemy::SpawnEnterPhaseTwoParticles()
{
	if (PhaseTwoParticles && GetWorld())
	{
		FVector SpawnLocation = GetActorLocation() - FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		//리치 발 밑에 생성되도록

		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			PhaseTwoParticles,
			SpawnLocation
		);
	}
}

void ALichEnemy::PlayEnterPhaseSound()
{
	if (EnterPhaseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EnterPhaseSound,
			GetActorLocation()
		);
	}
}

bool ALichEnemy::HasExistPlayerInFront()//플레이어 체크 함수
{
	TArray<FHitResult> HitResults;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * SwingSkillEnableRange;
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

bool ALichEnemy::HasExistSmashPlayerInFront()//플레이어 체크 함수
{
	TArray<FHitResult> HitResults;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * SmashSkillEnableRange;
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

bool ALichEnemy::HasExistMagicPlayerInFront()//플레이어 체크 함수
{
	TArray<FHitResult> HitResults;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * MagicSkillEnableRange;
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

bool ALichEnemy::HasExistTeleportPlayerInFront()//플레이어 체크 함수
{
	TArray<FHitResult> HitResults;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * TeleportSkillEnableRange;
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

void ALichEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)//타격 받는 함수
{
	if (IsEngaged() && IsHitOnShield(Hitter)) return;

	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if (!IsDead()) {
		ShowHealthBar();
	}
	ClearPatrolTimer();
	if (!bAction) {
		ClearAttackTimer();
	}
	EquippedWeapon->DeactivateLeftCastSkillEffect(); // 주문검 공격 이펙트 비활성화
	GetCharacterMovement()->Activate();
	EnemyState = EEnemyState::EES_Attacking;
	bAttack = true;
	//StopAttackMontage();

	if (IsInsideAttackRadius())
	{
		if (!IsDead()) StartAttackTimer();
	}
}

void ALichEnemy::GetStun_Implementation(const FVector& ImpactPoint, AActor* Hitter)//맞는 함수
{
	float AngleToMonster = CalculateAngleBetweenPlayerAndMonster(this, Hitter);
	if (!(IsEngaged() && AngleToMonster <= MaxParryAngle)) return;
	Super::GetStun_Implementation(ImpactPoint, Hitter);
	if (!IsDead()) {
		ShowHealthBar();
	}
	ClearPatrolTimer();
	if (!bAction) {
		ClearAttackTimer();
	}
	EquippedWeapon->DeactivateLeftCastSkillEffect(); 
	EnemyState = EEnemyState::EES_Stunned;
	StopAttackMontage();

	if (IsInsideAttackRadius())
	{
		if (!IsDead()) StartAttackTimer();
	}
}

void ALichEnemy::BallHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)//맞는 함수
{
	if (IsEngaged()) return;
	Super::BallHit_Implementation(ImpactPoint, Hitter);
	if (!IsDead()) {
		ShowHealthBar();
	}
	ClearPatrolTimer();
	ClearAttackTimer();
	EnemyState = EEnemyState::EES_Attacking;
	StopAttackMontage();
	UE_LOG(LogTemp, Log, TEXT("BallHit_Implementation"));
	if (IsInsideAttackRadius())
	{
		if (!IsDead()) StartAttackTimer();
	}
}

void ALichEnemy::SwingSkill()
{
	FTimerHandle RushSkillCountdown;
	FTimerHandle SkillCountdown;
	FTimerHandle CollisonCountdown;

	if (CanSwingAttack() && bCanSwingSkill && bCanSkill && !bAction) //스킬 우선순위 설정
	{
		PlaySwingSkillMontage();
		EnemyState = EEnemyState::EES_Engaged;
		bCanSwingSkill = false;//스윙 연속 사용 X
		bCanSkill = false;//스킬 연속 사용 X
		bAttack = false; //기본 공격 변수
		bAction = true; //행동 변수
		GetCharacterMovement()->Deactivate();
		GetWorldTimerManager().SetTimer(RushSkillCountdown, this, &ALichEnemy::EnableSwingSkill, SwingSkillCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(SkillCountdown, this, &ALichEnemy::EnableSkill, AllSkillCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(CollisonCountdown, this, &ALichEnemy::EndSwingSkill, CollisonTimer, false); // 쿨타임 타이머 시작
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//스윙 공격 시 무적
	}
}

void ALichEnemy::SwingSpellEffect()
{
	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FRotator PlayerRotation = GetActorRotation();
	FRotator DesiredRotation = PlayerRotation + FRotator(0.0f, 0.0f, 0.0f);
	FVector ForwardVector = GetActorForwardVector(); // 플레이어 전방
	FVector SpawnLocationA = GetActorLocation() + (ForwardVector * 200.0f) - FVector(0.0f, 0.0f, CapsuleHalfHeight);;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SwingImpactEffect, SpawnLocationA, DesiredRotation);
}

void ALichEnemy::SwingSpellSweepTrace()
{
	FVector Start = GetActorLocation() + GetActorForwardVector() * 100.0f; // 보스 바로 앞 위치에서 시작
	FVector ForwardVector = GetActorForwardVector(); // 보스가 향하는 방향
	FVector End = Start + ForwardVector * 300.0f; // 일정 거리만큼 스윕 트레이스
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
	UE_LOG(LogTemp, Log, TEXT("SwingSpellSweepTrace")); //확인완료

	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EndPointEffect, SpawnLocation, FRotator::ZeroRotator);
	if (bResult) {
		if (AActor* Actor = HitResult.GetActor()) {
			if (HitResult.GetActor()->ActorHasTag("EngageableTarget"))
			{
				UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *HitResult.GetActor()->GetName());
				//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.f, 0, 1.f);
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LichSwingHitEffect, HitResult.ImpactPoint, FRotator::ZeroRotator);
				PlayLichSwingHitSound(HitResult.ImpactPoint);
				FDamageEvent DamageEvent;

				UGameplayStatics::ApplyDamage(HitResult.GetActor(), LichDamage, GetInstigatorController(), this, UDamageType::StaticClass());
				ExecuteGetHit(HitResult);
				ExecuteGetBlock(HitResult);
			}
		}
	}
	else {
		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.f, 0, 1.f);
	}
}

void ALichEnemy::EndSwingSkill()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제
}

void ALichEnemy::EnableSwingSkill()
{
	bCanSwingSkill = true;// 스윙 공격 사용 활성화
}

void ALichEnemy::EnableSkill()
{
	bCanSkill = true;// 스킬 사용 활성화
}

void ALichEnemy::DisableSwingSkill()
{
	bCanSwingSkill = false;// 스윙 공격 사용 비활성화
}

void ALichEnemy::DeactivateSwingSkillEffect()
{
	EquippedWeapon->DeactivateLichSwingAttackEffect(); // 스윙 이펙트 비활성화
}

void ALichEnemy::ActivateSwingSkillEffect()
{
	EquippedWeapon->ActivateLichSwingAttackEffect(); // 스윙 이펙트 활성화
}

void ALichEnemy::PlayLichSwingHitSound(const FVector& ImpactPoint)
{
	if (LichSwingHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			LichSwingHitSound,
			ImpactPoint
		);
	}
}

void ALichEnemy::PlayLichSwingSound()
{
	if (LichSwingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			LichSwingSound,
			GetActorLocation()
		);
	}
}

void ALichEnemy::TeleportSkill()
{
	FTimerHandle TeleportSkillCountdown;
	FTimerHandle TeleAllSkillCountdown;
	FTimerHandle TeleportCollisonCountdown;

	if (CanSmashAttack() && !bCanSwingSkill && !bCanSmashSkill && bCanTeleportSkill && bCanSkill && !bAction) //스킬 우선순위 설정
	{
		PlayTeleportSkillMontage();
		EnemyState = EEnemyState::EES_Engaged;
		bCanTeleportSkill = false;//텔레포트 연속 사용 X
		bCanSkill = false;//스킬 연속 사용 X
		bAttack = false;
		bAction = true;
		GetCharacterMovement()->Deactivate();
		GetWorldTimerManager().SetTimer(TeleportSkillCountdown, this, &ALichEnemy::EnableTeleportSkill, TeleportSkillCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(TeleAllSkillCountdown, this, &ALichEnemy::EnableSkill, AllSkillCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(TeleportCollisonCountdown, this, &ALichEnemy::EndTeleportSkill, CollisonTimer, false); // 쿨타임 타이머 시작
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//텔레포트 공격 시 무적
	}
}

void ALichEnemy::TeleportToPlayer()
{
	TArray<FHitResult> HitResults;
	FVector StartLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();
	FVector EndLocation = StartLocation + GetActorForwardVector() * TeleportSkillEnableRange;
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
					FVector TeleportLocation = ProjectNo1Character->GetActorLocation() - (ForwardVector * TeleportDistance); // TeleportDistance는 텔레포트 거리를 나타냅니다.
					SetActorLocation(TeleportLocation);
				}
			}
		}
	}
}

void ALichEnemy::EndTeleportSkill()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제
}

void ALichEnemy::EnableTeleportSkill()
{
	bCanTeleportSkill = true;// 텔레포트 공격 사용 활성화
}

void ALichEnemy::DisableTeleportSkill()
{
	bCanTeleportSkill = false;// 텔레포트 공격 사용 비활성화
}

void ALichEnemy::TeleportSpellEffect()
{
	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FRotator PlayerRotation = GetActorRotation();
	FRotator DesiredRotation = PlayerRotation + FRotator(0.0f, 0.0f, 180.0f);
	FVector SpawnLocation = GetActorLocation() - FVector(0.0f, 0.0f, CapsuleHalfHeight);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TeleportEffect, SpawnLocation, DesiredRotation);
}

void ALichEnemy::SmashSkill()
{
	FTimerHandle SmashSkillCountdown;
	FTimerHandle CommonSkillCountdown;
	FTimerHandle SmashCollisonCountdown;

	if (CanSmashAttack() && !bCanSwingSkill && bCanSmashSkill && bCanSkill && bIsPhaseTwo && !bAction) //스킬 우선순위 설정
	{
		PlaySmashSkillMontage();
		EnemyState = EEnemyState::EES_Engaged;
		bCanSmashSkill = false;//스매쉬 연속 사용 X
		bCanSkill = false;//스킬 연속 사용 X
		bAttack = false;
		bAction = true;
		GetCharacterMovement()->Deactivate();
		GetWorldTimerManager().SetTimer(SmashSkillCountdown, this, &ALichEnemy::EnableSmashSkill, SmashSkillCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(CommonSkillCountdown, this, &ALichEnemy::EnableSkill, AllSkillCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(SmashCollisonCountdown, this, &ALichEnemy::EndSmashSkill, CollisonTimer, false); // 쿨타임 타이머 시작
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//스매쉬 공격 시 무적
	}
}

void ALichEnemy::SmashSpellEffect()
{
	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FRotator PlayerRotation = GetActorRotation();
	FRotator DesiredRotation = PlayerRotation + FRotator(0.0f, 0.0f, 0.0f);
	FVector ForwardVector = GetActorForwardVector(); // 플레이어 전방
	FVector SpawnLocationA = GetActorLocation() + (ForwardVector * 200.0f) - FVector(0.0f, 0.0f, CapsuleHalfHeight);;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmashImpactEffect, SpawnLocationA, DesiredRotation);
}

void ALichEnemy::SmashSpellSweepTrace()
{
	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();//발바닥 높이로 맞추기
	FVector Start = GetActorLocation() + GetActorForwardVector() * 100.0f; // 보스 바로 앞 위치에서 시작
	FVector ForwardVector = GetActorForwardVector(); // 보스가 향하는 방향
	FVector End = Start + ForwardVector * 500.0f; // 일정 거리만큼 스윕 트레이스
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
	UE_LOG(LogTemp, Log, TEXT("SmashSpellSweepTrace")); //확인완료

	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EndPointEffect, SpawnLocation, FRotator::ZeroRotator);

	if (bResult) {
		if (AActor* Actor = HitResult.GetActor()) {
			if (HitResult.GetActor()->ActorHasTag("EngageableTarget"))
			{
				UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *HitResult.GetActor()->GetName());
				//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.f, 0, 1.f);

				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LichSmashHitEffect, HitResult.ImpactPoint - FVector(0.0f, 0.0f, 80.0f), FRotator::ZeroRotator);
				PlayLichSmashHitSound(HitResult.ImpactPoint);
				FDamageEvent DamageEvent;

				UGameplayStatics::ApplyDamage(HitResult.GetActor(), LichDamage, GetInstigatorController(), this, UDamageType::StaticClass());
				ExecuteGetHit(HitResult);
				ExecuteGetBlock(HitResult);
			}
		}
	}
	else {
		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.f, 0, 1.f);
	}
}

void ALichEnemy::EndSmashSkill()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제
}

void ALichEnemy::EnableSmashSkill()
{
	bCanSmashSkill = true;// 스매쉬 공격 사용 활성화
}

void ALichEnemy::DisableSmashSkill()
{
	bCanSmashSkill = false;// 스매쉬 공격 사용 비활성화
}

void ALichEnemy::DeactivateSmashSkillEffect()
{
	EquippedWeapon->DeactivateLichSmashAttackEffect(); // 스매쉬 이펙트 비활성화
}

void ALichEnemy::ActivateSmashSkillEffect()
{
	EquippedWeapon->ActivateLichSmashAttackEffect(); // 스매쉬 이펙트 활성화
}

void ALichEnemy::PlayLichSmashHitSound(const FVector& ImpactPoint)
{
	if (LichSmashHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			LichSmashHitSound,
			ImpactPoint
		);
	}
}

void ALichEnemy::PlayLichSmashSound()
{
	if (LichSmashSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			LichSmashSound,
			GetActorLocation()
		);
	}
}

void ALichEnemy::MagicSkill()
{
	FTimerHandle MagicSkillCountdown;
	FTimerHandle MagicCommonSkillCountdown;
	FTimerHandle MagicCollisonCountdown;

	if (CanMagicAttack() && bCanMagicSkill && bCanSkill && bIsPhaseTwo && !bAction) //스킬 우선순위 설정
	{
		PlayMagicSkillMontage();
		EnemyState = EEnemyState::EES_Engaged;
		bCanMagicSkill = false;//스매쉬 연속 사용 X
		bCanSkill = false;//스킬 연속 사용 X
		bAttack = false;
		bAction = true;
		GetCharacterMovement()->Deactivate();
		GetWorldTimerManager().SetTimer(MagicSkillCountdown, this, &ALichEnemy::EnableMagicSkill, MagicSkillCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(MagicCommonSkillCountdown, this, &ALichEnemy::EnableSkill, AllSkillCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(MagicCollisonCountdown, this, &ALichEnemy::EndMagicSkill, CollisonTimer, false); // 쿨타임 타이머 시작
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//스매쉬 공격 시 무적
	}
}

void ALichEnemy::MagicSpellEffect()
{
	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FRotator PlayerRotation = GetActorRotation();
	FRotator DesiredRotation = PlayerRotation + FRotator(0.0f, 0.0f, 0.0f);
	FVector ForwardVector = GetActorForwardVector(); // 플레이어 전방
	FVector SpawnLocationA = GetActorLocation() + (ForwardVector * 200.0f) - FVector(0.0f, 0.0f, CapsuleHalfHeight);;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MagicImpactEffect, SpawnLocationA, DesiredRotation);
}

void ALichEnemy::MagicSpellSweepTrace()
{
	FVector Start = GetActorLocation() + GetActorForwardVector() * 100.0f; // 보스 바로 앞 위치에서 시작
	FVector ForwardVector = GetActorForwardVector(); // 보스가 향하는 방향
	FVector End = Start + ForwardVector * 300.0f; // 일정 거리만큼 스윕 트레이스
	FVector SpawnLocation = GetActorLocation();
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
	UE_LOG(LogTemp, Log, TEXT("MagicSpellSweepTrace")); //확인완료

	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EndPointEffect, SpawnLocation, FRotator::ZeroRotator);
	if (bResult) {
		if (AActor* Actor = HitResult.GetActor()) {
			if (HitResult.GetActor()->ActorHasTag("EngageableTarget"))
			{
				UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *HitResult.GetActor()->GetName());
				//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.f, 0, 1.f);
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LichMagicHitEffect, HitResult.ImpactPoint, FRotator::ZeroRotator);
				PlayLichMagicHitSound(HitResult.ImpactPoint);
				FDamageEvent DamageEvent;

				UGameplayStatics::ApplyDamage(HitResult.GetActor(), LichDamage, GetInstigatorController(), this, UDamageType::StaticClass());
				ExecuteGetHit(HitResult);
				ExecuteGetBlock(HitResult);
			}
		}
	}
	else {
		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.f, 0, 1.f);
	}
}

void ALichEnemy::EndMagicSkill()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제
}

void ALichEnemy::EnableMagicSkill()
{
	bCanMagicSkill = true;// 스매쉬 공격 사용 활성화
}

void ALichEnemy::DisableMagicSkill()
{
	bCanMagicSkill = false;// 스매쉬 공격 사용 비활성화
}

void ALichEnemy::PlayLichMagicHitSound(const FVector& ImpactPoint)
{
	if (LichMagicHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			LichMagicHitSound,
			ImpactPoint
		);
	}
}

void ALichEnemy::PlayLichMagicSound()
{
	if (LichMagicSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			LichMagicSound,
			GetActorLocation()
		);
	}
}

bool ALichEnemy::IsHitOnShield(AActor* Hitter)
{
	// 여기에 방패에 맞았는지 여부를 확인하는 코드를 추가
	// 방패에 맞았다면 true를 반환하고, 그렇지 않다면 false를 반환
	// 예: Hitter이 방패 클래스에 속하는지 여부를 확인하는 코드
	return Hitter && Hitter->IsA(AShield::StaticClass());
}

void ALichEnemy::TakeExecutionHold()
{
		// 몬스터 움직임 고정
		GetCharacterMovement()->DisableMovement();
}

void ALichEnemy::TakeBack() //넉백 함수
{
	// 다시 몬스터 움직임 활성화
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void ALichEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void ALichEnemy::SpawnDefaultWeaponTwo()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("LeftCastWeapon"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void ALichEnemy::Die()
{
	Super::Die();

	EnemyState = EEnemyState::EES_Dead;
	ClearAttackTimer();
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetActorEnableCollision(false);
	SpawnEx();
	SpawnGd();
}

void ALichEnemy::DieState()
{
	EnemyState = EEnemyState::EES_Dead;
}

void ALichEnemy::SpawnEx()
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
void ALichEnemy::SpawnGd()
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

float ALichEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) //������ �ޱ�
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

void ALichEnemy::Destroyed()
{
	Super::Destroyed();
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
	OnBossDestroyedDelegate.Broadcast();
}

void ALichEnemy::StoreHitNumber(UUserWidget* HitNubmer, FVector Location)
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

void ALichEnemy::DestroyHitNumber(UUserWidget* HitNumber)
{
	HitNumbers.Remove(HitNumber);
	HitNumber->RemoveFromParent();
}

void ALichEnemy::UpdateHitNumbers()
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

void ALichEnemy::PatrolTimerFinished()
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
}

void ALichEnemy::PawnSeen(APawn* SeenPawn)
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

AActor* ALichEnemy::ChoosePatrolTarget()
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

void ALichEnemy::DeactivateLeftCastEffect()
{
	UE_LOG(LogTemp, Log, TEXT("DeactivateLeftCastEffect"));
	EquippedWeapon->DeactivateLeftCastSkillEffect(); // 주문검 공격 이펙트 비활성화
}


void ALichEnemy::ActivateLeftCastEffect()
{
	FTimerHandle LeftCastSkillCountdown;

	EquippedWeapon->ActivateLeftCastSkillEffect(); // 주문검  공격 이펙트 활성화
	GetWorldTimerManager().SetTimer(LeftCastSkillCountdown, this, &ALichEnemy::DeactivateLeftCastEffect, LeftCastSkillCount, false); // 쿨타임 타이머 시작
}

void ALichEnemy::AttackSweepTrace()
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
		FCollisionShape::MakeSphere(60.0f),
		CollisionParams);
	UE_LOG(LogTemp, Log, TEXT("LichAttackSweepTrace")); //확인완료

	//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EndPointEffect, SpawnLocation, FRotator::ZeroRotator);
	if (bResult) {
		if (AActor* Actor = HitResult.GetActor()) {
			if (HitResult.GetActor()->ActorHasTag("EngageableTarget"))
			{
				UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *HitResult.GetActor()->GetName());

				//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, HitResult.ImpactPoint, FRotator::ZeroRotator);
				//PlayWeaponSpellHitSound(HitResult.ImpactPoint);
				FDamageEvent DamageEvent;

				UGameplayStatics::ApplyDamage(HitResult.GetActor(), LichDamage, GetInstigatorController(), this, UDamageType::StaticClass());
				ExecuteGetHit(HitResult);
				ExecuteGetBlock(HitResult);
			}
		}
	}
	else {
		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.f, 0, 1.f);
	}
}

void ALichEnemy::ExecuteGetHit(FHitResult& HitResult)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(HitResult.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetHit(HitResult.GetActor(), HitResult.ImpactPoint, GetOwner());
	}
}

void ALichEnemy::ExecuteGetBlock(FHitResult& HitResult)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(HitResult.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetBlock(HitResult.GetActor(), HitResult.ImpactPoint, GetOwner());
	}
}

void ALichEnemy::Attack()
{
	Super::Attack();
	if (CombatTarget == nullptr || !bAttack || bAction) return;
	GetCharacterMovement()->Deactivate();
	EnemyState = EEnemyState::EES_Engaged;
	PlayAttackMontage();
}

bool ALichEnemy::CanAttack()
{
	bool bCanAttack =
		IsInsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsStunned() &&
		!IsDead();
	return bCanAttack;
}

bool ALichEnemy::CanSwingAttack()
{
	bool bCanLaserAttack =
		IsOutsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsStunned() &&
		!IsDead();
	return bCanLaserAttack;
}

bool ALichEnemy::CanSmashAttack()
{
	bool bCanRushAttack =
		IsOutsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsStunned() &&
		!IsDead();
	return bCanRushAttack;
}

bool ALichEnemy::CanMagicAttack()
{
	bool bCanMagicAttack =
		IsOutsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsStunned() &&
		!IsDead();
	return bCanMagicAttack;
}

void ALichEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	GetCharacterMovement()->Activate();
	CheckCombatTarget();
	bAttack = true;
	bAction = false;
}

void ALichEnemy::HitEnd()
{
	EnemyState = EEnemyState::EES_Attacking;
	GetCharacterMovement()->Activate();//이동 허용
	bAttack = true;
}

void ALichEnemy::StunEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	GetCharacterMovement()->Activate();//이동 허용
	bAttack = true;	// 공격 가능 상태로 전환
}

void ALichEnemy::StunStart()
{
	EnemyState = EEnemyState::EES_Stunned;
	GetCharacterMovement()->Deactivate(); //이동 억제
	bAttack = false; 	// 공격 불가능 상태로 전환
}

void ALichEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void ALichEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	HideHealthBar();
	SpawnDefaultWeapon();
	SpawnDefaultWeaponTwo();
}

void ALichEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void ALichEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

void ALichEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void ALichEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void ALichEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}


void ALichEnemy::CombatTargetPlayer()
{
	EnemyState = EEnemyState::EES_Chasing;
	UE_LOG(LogTemp, Log, TEXT("PlayerCombat"));
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(ProjectNo1Character);
}


bool ALichEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool ALichEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool ALichEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool ALichEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool ALichEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool ALichEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool ALichEnemy::Alive()
{
	return Attributes && Attributes->IsAlive();
}

bool ALichEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

bool ALichEnemy::IsStunned()
{
	return EnemyState == EEnemyState::EES_Stunned;
}

bool ALichEnemy::IsNotStunned()
{
	return EnemyState != EEnemyState::EES_Stunned;
}

void ALichEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void ALichEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &ALichEnemy::Attack, AttackTime);
}

void ALichEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

// 발사체 액터 생성 함수
void ALichEnemy::ProjectileAttack()
{
	// 화살이 발사될 위치를 설정한다
	FVector vPos = GetActorLocation() + GetActorUpVector() * 30.f + GetActorForwardVector() * 100.f;

	// 스폰 파라미터를 설정한다
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 월드에 액터를 배치한다
	AProjectileWeapon* ProjectileWeapon = GetWorld()->SpawnActor<AProjectileWeapon>(ProjectileWeaponClass, vPos, GetActorRotation(), params);
}
