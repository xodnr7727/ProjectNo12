// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/BaseCharacter.h"
#include "CharacterTypes.h"
#include "Components/StaticMeshComponent.h"
#include "Templates/UnrealTemplate.h"
#include "Components/BoxComponent.h"
#include "Weapons/ProjectileWeapon.h"
#include "LichEnemy.generated.h"
class UHealthBarComponent;
class UPawnSensingComponent;
class AProjectNo1Character;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSwingSkillDetected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSmashSkillDetected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMagicSkillDetected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeleportSkillDetected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDestroyedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossPhaseDelegated);

UCLASS()
class PROJECTNO1_API ALichEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:

	ALichEnemy();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSwingSkillDetected OnSwingSkillDetected;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSmashSkillDetected OnSmashSkillDetected;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMagicSkillDetected OnMagicSkillDetected;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTeleportSkillDetected OnTeleportSkillDetected;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBossDestroyedDelegate OnBossDestroyedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBossPhaseDelegated OnBossPhaseDelegated;

protected:

	virtual void BeginPlay() override;

	/** <ABaseCharacter> */
	virtual void Die() override;
	void SpawnEx();//경험치 드랍
	void SpawnGd();//골드 드랍
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void AttackEnd() override;

	UFUNCTION(BlueprintCallable)
	void DeactivateLeftCastEffect();

	UFUNCTION(BlueprintCallable)
	void ActivateLeftCastEffect();

	//기본 공격
	UFUNCTION(BlueprintCallable)
	void AttackSweepTrace();

	//히트, 블록 관련
	void ExecuteGetHit(FHitResult& HitResult);
	void ExecuteGetBlock(FHitResult& HitResult);
	virtual void HitEnd() override;
	virtual void StunEnd() override;
	virtual void StunStart() override;

		/* <스윙 패턴 관련> */
	UFUNCTION(BlueprintCallable, Category = "SpecialSkill")
	void SwingSkill();

	UFUNCTION(BlueprintCallable)
	void SwingSpellEffect();

	UFUNCTION(BlueprintCallable)
	void SwingSpellSweepTrace();

	UFUNCTION(BlueprintCallable)
	void DeactivateSwingSkillEffect();

	UFUNCTION(BlueprintCallable)
	void ActivateSwingSkillEffect();

	virtual bool CanSwingAttack() override;
	void EndSwingSkill();
	void EnableSwingSkill();
	void DisableSwingSkill();
	void CheckForSwing();

	void PlayLichSwingHitSound(const FVector& ImpactPoint);

	UFUNCTION(BlueprintCallable)
	void PlayLichSwingSound();

	/* <텔레포트 패턴 관련> */
	UFUNCTION(BlueprintCallable, Category = "SpecialSkill")
	void TeleportSkill();
	UFUNCTION(BlueprintCallable)
	void TeleportToPlayer();
	UFUNCTION(BlueprintCallable)
	void TeleportSpellEffect();

	void EndTeleportSkill();
	void EnableTeleportSkill();
	void DisableTeleportSkill();
	void CheckForTeleport();

	/* <스매쉬 패턴 관련> */
	UFUNCTION(BlueprintCallable, Category = "SpecialSkill")
	void SmashSkill();
	UFUNCTION(BlueprintCallable)
	void SmashSpellEffect();
	UFUNCTION(BlueprintCallable)
	void SmashSpellSweepTrace();
	UFUNCTION(BlueprintCallable)
	void DeactivateSmashSkillEffect();
	UFUNCTION(BlueprintCallable)
	void ActivateSmashSkillEffect();
	void PlayLichSmashHitSound(const FVector& ImpactPoint);

	UFUNCTION(BlueprintCallable)
	void PlayLichSmashSound();

	virtual bool CanSmashAttack() override;
	void EndSmashSkill();
	void EnableSmashSkill();
	void DisableSmashSkill();
	void CheckForSmash();

	/* <매직 미사일 패턴 관련> */
	UFUNCTION(BlueprintCallable, Category = "SpecialSkill")
	void MagicSkill();
	UFUNCTION(BlueprintCallable)
	void MagicSpellEffect();
	UFUNCTION(BlueprintCallable)
	void MagicSpellSweepTrace();

	virtual bool CanMagicAttack() override;
	void EndMagicSkill();
	void EnableMagicSkill();
	void DisableMagicSkill();
	void CheckForMagic();

	void PlayLichMagicHitSound(const FVector& ImpactPoint);

	UFUNCTION(BlueprintCallable)
	void PlayLichMagicSound();

	void EnableSkill(); //스킬 활성화

	UFUNCTION()
	void DestroyHitNumber(UUserWidget* HitNumber);

	void UpdateHitNumbers();

	UPROPERTY(EditAnywhere, Category = Combat)
		float DeathLifeSpan = 1.0f;

	UFUNCTION()
		void PawnSeen(APawn* SeenPawn);

	UPROPERTY(BlueprintReadOnly)
		EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	UPROPERTY(VisibleAnywhere)
		UPrimitiveComponent* MyCollisionComponent;

		UPROPERTY(Category = ArrowAttack, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AProjectileWeapon> ProjectileWeaponClass;

public:	

	virtual void Tick(float DeltaTime) override;

	/** 2 페이즈 */
	void CheckHealth();
	UFUNCTION()
	void EnterPhaseTwo();
	void ResetPositions();
	void EnterEndPhaseTwo();
	UFUNCTION(BlueprintCallable)
	void ChangeBossAppearance();
	UFUNCTION(BlueprintCallable)
	void SpawnEnterPhaseTwoParticles();
	UFUNCTION(BlueprintCallable)
	void PlayEnterPhaseSound();

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;//히트 함수
	virtual void GetStun_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;//스턴 함수
	virtual void BallHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	bool IsHitOnShield(AActor* Hitter);
	void TakeExecutionHold(); //홀딩 상태
	void TakeBack(); //홀딩 해제
	
	virtual void Destroyed() override;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 Damage, FVector HitLocation);

	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* HitNubmer, FVector Location);
	bool Alive();
	bool IsStunned();//스턴됨
	bool IsNotStunned();//스턴풀림
	bool IsEngaged();//전투중
	void InitializeEnemy();
	void ShowHealthBar();
	void CombatTargetPlayer();
	void CheckPatrolTarget();
	void CheckCombatTarget();

	void ProjectileAttack();

	FORCEINLINE EEnemyState GetEnemyState() const { return EnemyState; }

private:

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
		float PatrolWaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
		float PatrolWaitMax = 10.f;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
		double AttackRadius = 150.f;

	UPROPERTY(EditAnywhere)
		double RushRadius = 400.f;

	ACharacter* PlayerCharacter;

	UPROPERTY()
		class AAIController* EnemyController;

	// Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		TArray<AActor*> PatrolTargets;

	UPROPERTY(VisibleAnywhere)
		UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
		UPawnSensingComponent* PawnSensing;

		//맵에 타격 데미지를 적중 위치 저장
	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> HitNumbers;

	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitNumberDestroyTime;

	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
		class UBehaviorTree* BehaviorTree;

	/** AI behavior */
	void HideHealthBar();
	void LoseInterest();//추적 X
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();//공격 사정거리 밖
	bool IsInsideAttackRadius();//공격 사정거리 안
	bool IsChasing();//쫓는중
	bool IsAttacking();//히트됨
	bool IsDead();//죽음
	void ClearPatrolTimer();
	void SpawnDefaultWeapon();
	void SpawnDefaultWeaponTwo();

	/* 플레이어 감지 */
	UFUNCTION(BlueprintPure, Category = "Skill")
	bool HasExistPlayerInFront();

	UFUNCTION(BlueprintPure, Category = "Skill")
	bool HasExistSmashPlayerInFront();

	UFUNCTION(BlueprintPure, Category = "Skill")
	bool HasExistMagicPlayerInFront();

	UFUNCTION(BlueprintPure, Category = "Skill")
	bool HasExistTeleportPlayerInFront();

	/** Combat */
	void StartAttackTimer();
	void ClearAttackTimer();

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
		float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = Combat)
		float AttackMax = 1.f;

	UPROPERTY(EditAnywhere, Category = Combat)
		float PatrollingSpeed = 125.f;

	UPROPERTY(EditAnywhere, Category = Combat)
		float ChasingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Attack Properties")
		float LichDamage;

	UPROPERTY(EditAnywhere, Category = Combat)
		TSubclassOf<class ASoul> ExClass;

	UPROPERTY(EditAnywhere, Category = Combat)
		TSubclassOf<class ATreasure> GdClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(VisibleAnywhere, Category = Player)
		AProjectNo1Character* ProjectNo1Character;

	UPROPERTY(EditAnywhere, Category = "Skill")
		bool bCanSkill; // 스킬 사용할 수 있는지 여부
	UPROPERTY(EditAnywhere, Category = "Skill")
		float AllSkillCooldown; // 스킬 쿨타임 변수
	UPROPERTY(EditAnywhere, Category = "Skill")
		float CollisonTimer; // 콜리전 해제 타이머

	UPROPERTY(EditAnywhere, Category = "Skill")
		bool bAttack;

	UPROPERTY(EditAnywhere, Category = "Skill")
		bool bAction;

	UPROPERTY(EditAnywhere, Category = "Skill")
		float LeftCastSkillCount;//  지속 시간

		/* <스매쉬 패턴 관련> */
	UPROPERTY(EditAnywhere, Category = "Skill")
	float SmashSkillCooldown; // 스매시 스킬 쿨타임 변수
	bool bCanSmashSkill; // 스매시 스킬 사용할 수 있는지 여부

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float SmashSkillEnableRange;  // Range to detect player in front of the enemie

	UPROPERTY(EditAnywhere, Category = "Skill")
	USoundBase* LichSmashHitSound;

	UPROPERTY(EditAnywhere, Category = "Skill")
	USoundBase* LichSmashSound;

	UPROPERTY(EditAnywhere, Category = "Skill")
	UParticleSystem* LichSmashHitEffect;

	UPROPERTY(EditAnywhere, Category = "Skill")
	UParticleSystem* SmashImpactEffect;

	/* <텔레포트 패턴 관련> */
	UPROPERTY(EditAnywhere, Category = "Skill")
	UParticleSystem* TeleportEffect;

	UPROPERTY(EditAnywhere, Category = "Skill")
	float TeleportSkillCooldown; // 스윙 스킬 쿨타임 변수
	bool bCanTeleportSkill;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float TeleportSkillEnableRange;  // Range to detect player in front of the enemie

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float TeleportDistance;
	
	/* <스윙 패턴 관련> */
	UPROPERTY(EditAnywhere, Category = "Skill")
	float SwingSkillCooldown; // 스윙 스킬 쿨타임 변수
	bool bCanSwingSkill; // 스윙 스킬 사용할 수 있는지 여부

	UPROPERTY(EditAnywhere, Category = "Skill")
	USoundBase* LichSwingSound;

	UPROPERTY(EditAnywhere, Category = "Skill")
	USoundBase* LichSwingHitSound;

	UPROPERTY(EditAnywhere, Category = "Skill")
	UNiagaraSystem* LichSwingHitEffect;

	UPROPERTY(EditAnywhere, Category = "Skill")
	UParticleSystem* SwingImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float SwingSkillEnableRange;  // Range to detect player in front of the enemie

	/* <매직 미사일 패턴 관련> */
	UPROPERTY(EditAnywhere, Category = "Skill")
	float MagicSkillCooldown; // 매직 미사일 스킬 쿨타임 변수
	bool bCanMagicSkill; // 매직 미사일 스킬 사용할 수 있는지 여부

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float MagicSkillEnableRange;  // Range to detect player in front of the enemie

	UPROPERTY(EditAnywhere, Category = "Skill")
	USoundBase* LichMagicHitSound;

	UPROPERTY(EditAnywhere, Category = "Skill")
	USoundBase* LichMagicSound;

	UPROPERTY(EditAnywhere, Category = "Skill")
	UParticleSystem* LichMagicHitEffect;

	UPROPERTY(EditAnywhere, Category = "Skill")
	UParticleSystem* MagicImpactEffect;

	/* <2페이즈 관련> */
	UPROPERTY(EditAnywhere, Category = "Boss")
	USkeletalMesh* PhaseTwoMesh;

	bool bIsPhaseTwo;

	UPROPERTY(EditAnywhere, Category = "Boss")
	float PhaseTwoTimer; // 페이즈 입장 시간 변수

	UPROPERTY(EditAnywhere, Category = "Boss")
	FVector InitialLocation;

	UPROPERTY(EditAnywhere, Category = "Boss")
	USoundBase* EnterPhaseSound;

	UPROPERTY(EditAnywhere, Category = "Boss")
	UParticleSystem* PhaseTwoParticles;

};
