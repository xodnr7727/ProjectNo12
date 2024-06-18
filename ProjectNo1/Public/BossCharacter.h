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
#include "BossCharacter.generated.h"
class UHealthBarComponent;
class UStunBarComponent;
class UPawnSensingComponent;
class AProjectNo1Character;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLaserSkillDetected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRushSkillDetected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDestroyedDelegate);

/**
 * 
 */
UCLASS()
class PROJECTNO1_API ABossCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	ABossCharacter();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLaserSkillDetected OnLaserSkillDetected;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRushSkillDetected OnRushSkillDetected;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBossDestroyedDelegate OnBossDestroyedDelegate;
protected:

	virtual void BeginPlay() override;

	/** <ABaseCharacter> */
	virtual void Die() override;
	void SpawnEx();
	void SpawnGd();
	bool InTargetRange(AActor* Target, double Radius);
	AActor* ChoosePatrolTarget();
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual bool CanLaserAttack() override;
	virtual bool CanRushAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void AttackEnd() override;
	virtual void HitEnd() override;
	virtual void StunEnd() override;
	virtual void StunStart() override;

	UFUNCTION(BlueprintCallable)
	void RecoveryStunState();

	UFUNCTION(BlueprintCallable, Category = "SpecialSkill")
	void OnLaserSkill();
	void IncreaseDamage();
	void RestoreDamage();
	virtual void EndLaserSkill() override;
	void EnableLaserSkill();

	UFUNCTION(BlueprintCallable)
	void ActivateLaserSkillEffect();

	UFUNCTION(BlueprintCallable)
	void RushSpellSweepTrace();

	void ExecuteGetHit(FHitResult& HitResult);

	void ExecuteGetBlock(FHitResult& HitResult);

	UFUNCTION(BlueprintCallable, Category = "SpecialSkill")
	void RushSkill();
	virtual void EndRushSkill() override;
	void EnableRushSkill();
	void EnableSkill();
	void DisableRushSkill();

	void CheckForLaser();
	void CheckForRush();

	UFUNCTION(BlueprintCallable)
	void ActivateRushSkillEffect();

	UFUNCTION(BlueprintCallable)
	void DeactivateRushSkillEffect();

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
	void CheckPatrolTarget();
	void CheckCombatTarget();

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void BallHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void GetStun_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 Damage, FVector HitLocation);

	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* HitNubmer, FVector Location);
	bool Alive();
	bool IsCanLaserSkill();
	void Stun();
	void ProjectileAttack();
	void InitializeEnemy();
	void ShowHealthBar();
	void ShowStunBar();
	void MoveToTarget(AActor* Target);
	void ChaseTarget();
	void CombatTargetPlayer();

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
	double AttackRadius = 400.f;

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
	UStunBarComponent* StunBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> LaserWeaponClass;

	//맵에 타격 데미지를 적중 위치 저장
	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> HitNumbers;

	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitNumberDestroyTime;

	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;


	/** AI behavior */
	void HideHealthBar();
	void HideStunBar();
	void LoseInterest();
	void StartPatrolling();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsStunned();
	bool IsEngaged();
	void ClearPatrolTimer();

	// Function to check if there is a stunned enemy in front of the player
	UFUNCTION(BlueprintPure, Category = "Skill")
	bool HasExistPlayerInFront();

	UFUNCTION(BlueprintPure, Category = "Skill")
	bool HasExistRushPlayerInFront();

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

	UPROPERTY(EditAnywhere)
	bool bAttack;

	UPROPERTY(EditAnywhere, Category = "Attack Properties")
	float BossDamage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class ASoul> ExClass;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class ATreasure> GdClass;

	UPROPERTY(VisibleAnywhere, Category = Player)
	AProjectNo1Character* ProjectNo1Character;

	bool bCanSkill; // 스킬 사용할 수 있는지 여부
	float AllSkillCooldown; // 스킬 쿨타임 변수
	float CollisonTimer; // 콜리전 해제 타이머

	UPROPERTY(EditAnywhere, Category = "Skill")
	float LaserSkillCooldown; // 레이저 스킬 쿨타임 변수
	bool bCanLaserSkill; // 레이저 스킬 사용할 수 있는지 여부
	float LaserSkillTimer; // 레이저 지속시간

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float LaserSkillEnableRange;  // Range to detect player in front of the enemie

	UPROPERTY(VisibleAnywhere, Category = "Skill")
	UParticleSystemComponent* LaserSkillEffect;

	UPROPERTY(EditAnywhere, Category = "Skill")
	float RushSkillCooldown; // 돌진 스킬 쿨타임 변수
	bool bCanRushSkill; // 돌진 스킬 사용할 수 있는지 여부

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	float RushSkillEnableRange;  // Range to detect player in front of the enemie

	UPROPERTY(VisibleAnywhere, Category = "Skill")
	UParticleSystemComponent* RushSkillEffect;

};