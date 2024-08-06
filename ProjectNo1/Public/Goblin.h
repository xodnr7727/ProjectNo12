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
#include "Goblin.generated.h"
class UHealthBarComponent;
class UPawnSensingComponent;
class AProjectNo1Character;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDestroyedDelegate);

UCLASS()
class PROJECTNO1_API AGoblin : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGoblin();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDestroyedDelegate OnDestroyedDetected;

protected:

	virtual void BeginPlay() override;

	/** <ABaseCharacter> */
	virtual void Die() override;
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void AttackEnd() override;

	void RestartMonsterAtLocation(FVector RestartspawnLocation);
	void SetInitialSpawnLocation();
	void SpawnEx();
	void SpawnGd();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UFUNCTION(BlueprintCallable)
	void DieState();

	UFUNCTION(BlueprintCallable)
	void AttackSweepTrace();

	void ExecuteGetHit(FHitResult& HitResult);

	void ExecuteGetBlock(FHitResult& HitResult);

	UFUNCTION()
	void DestroyHitNumber(UUserWidget* HitNumber);

	void UpdateHitNumbers();

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

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Destroyed() override;

	void Respawn();
	bool IsDead();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 Damage, FVector HitLocation);

	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* HitNubmer, FVector Location);

	void ProjectileAttack();
	void InitializeEnemy();
	void ShowHealthBar();
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
	double AttackRadius = 150.f;

	UPROPERTY(EditAnywhere)
	double RushRadius = 400.f;

	ACharacter* PlayerCharacter; // 플레이어 캐릭터

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

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AShield> ShieldClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ATopArmor> AmorClass;

	//맵에 타격 데미지를 적중 위치 저장
	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> HitNumbers;

	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitNumberDestroyTime;

	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;


	/** AI behavior */
	void HideHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsEngaged();
	void ClearPatrolTimer();
	void SpawnDefaultWeapon();

	void SpawnDefaultWeaponTwo();

	void SpawnDefaultAmor();

	/** Combat */
	void StartAttackTimer();
	void ClearAttackTimer();

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 1.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrollingSpeed = 100.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ChasingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Attack Properties")
	float GoblinDamage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class ASoul> ExClass;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class ATreasure> GdClass;

	UPROPERTY(VisibleAnywhere, Category = Player)
	AProjectNo1Character* ProjectNo1Character;

	FVector RespawnLocation;
	float RespawnTime;
	bool bGoblinDead;
};
