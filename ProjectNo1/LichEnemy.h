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
UCLASS()
class PROJECTNO1_API ALichEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:

	ALichEnemy();

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
	virtual void HitEnd() override;
	virtual void StunEnd() override;
	virtual void StunStart() override;

	UFUNCTION(BlueprintCallable)
	void DeactivateLeftCastEffect();

	UFUNCTION(BlueprintCallable)
	void ActivateLeftCastEffect();

	UFUNCTION()
	void DestroyHitNumber(UUserWidget* HitNumber);

	void UpdateHitNumbers();

	UPROPERTY(EditAnywhere, Category = Combat)
		float DeathLifeSpan = 8.f;

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


	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;//히트 함수
	virtual void GetStun_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;//스턴 함수
	virtual void BallHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	bool IsHitOnShield(AActor* Hitter);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void Destroyed() override;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 Damage, FVector HitLocation);

	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* HitNubmer, FVector Location);
	bool Alive();
	bool IsStunned();//스턴됨
	bool IsNotStunned();//스턴풀림
	bool IsEngaged();//전투중
	void ProjectileAttack();
	void InitializeEnemy();
	void ShowHealthBar();

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

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AWeapon> WeaponClass;

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

	UPROPERTY(EditAnywhere, Category = Combat)
		TSubclassOf<class ASoul> ExClass;

	UPROPERTY(EditAnywhere, Category = Combat)
		TSubclassOf<class ATreasure> GdClass;

	UPROPERTY(EditAnywhere)
		bool bAttack;

	UPROPERTY(EditAnywhere, Category = "Skill")
		float LeftCastSkillCount;//  지속 시간

};
