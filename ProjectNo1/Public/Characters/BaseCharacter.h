#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"
class AShield;
class AWeapon;
class AProjectileWeapon;
class ATopArmor;
class UAttributeComponent;
class UAnimMontage;
class ALichEnemy;
UCLASS()
class PROJECTNO1_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void GetBlock_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void BallHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	void GetStun_Implementation(const FVector& ImpactPoint, AActor* Hitter);
	float CalculateAngleBetweenPlayerAndMonster(AActor* Player, AActor* Hitter);
	virtual void Attack();
	virtual void Die();
	void DirectionalHitReact(const FVector& ImpactPoint);
	void DirectionalHitStun(const FVector& ImpactPoint);

	UFUNCTION(BlueprintImplementableEvent)
	void HitReactBlock();

	UFUNCTION(BlueprintImplementableEvent)
	void AttackReactParry();

	/**
	* Play montage functions
	*/

	virtual void HandleDamage(float DamageAmount);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	void PlayArrowSound(const FVector& ImpactPoint);
	void SpawnArrowParticles(const FVector& ImpactPoint);
	void PlayBlockSound(const FVector& ImpactPoint);
	void SpawnBlockParticles(const FVector& ImpactPoint);

	void DisableCapsule();

	virtual bool CanAttack();
	virtual bool CanNeckSkill();

	UFUNCTION(BlueprintCallable)
	bool IsAlive();

	UFUNCTION(BlueprintCallable)
	bool IsStun();

	bool HasEnoughShieldStamina();
	void PlayHitReactMontage(const FName& SectionName);
	void PlayStunReactMontage(const FName& SectionName);
	void PlayBlockReactMontage();
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual int32 PlayStunMontage();
	virtual void PlayDiveMontage();
	virtual void PlayDrinkMontage();
	virtual void PlayNeckSkillMontage();
	virtual void PlaySwordSkillMontage();
	void StopAttackMontage();

	void SpawnEffect(const FVector& ImpactPoint);

	FVector GetWeaponTraceEndLocation() const;

	UFUNCTION(BlueprintCallable)
		FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
		FVector GetRotationWarpTarget();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd(); 

	UFUNCTION(BlueprintCallable)
	virtual void BlockEnd(); 

	UFUNCTION(BlueprintCallable)
	virtual void  HitEnd();

	UFUNCTION(BlueprintCallable)
	virtual void StunEnd();

	UFUNCTION(BlueprintCallable)
	virtual void StunStart();

	UFUNCTION(BlueprintCallable)
		virtual void DiveEnd(); 

	UFUNCTION(BlueprintCallable)
		void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
		void SetShieldCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapon)
		AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapon)
		AProjectileWeapon* Projectile;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Shield)
		AShield* EquippedShield;

	UPROPERTY(VisibleAnywhere, Category = Enemy)
		ALichEnemy* LichEnemy;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	    AWeapon* EquippedNeck;

		UPROPERTY(VisibleAnywhere, Category = Weapon)
		ATopArmor* EquippedArmor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UAttributeComponent* Attributes;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
		AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = Combat)
		double WarpTargetDistance = 75.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
		float MaxParryAngle = 100;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Block")
		UAnimMontage* BlockMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* StunMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* NeckSkillMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* SwordSkillMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* DiveMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* DrinkMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
		TArray<FName> AttackMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
		TArray<FName> DeathMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
		TArray<FName> StunMontageSections;
	/*
	* Components
	*/

private:
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);

	UPROPERTY(EditAnywhere, Category = Combat)
		USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = Combat)
		UParticleSystem* HitParticles;

		UPROPERTY(EditAnywhere, Category = Combat)
		USoundBase* BlockSound;

		UPROPERTY(EditAnywhere, Category = Combat)
		UParticleSystem* BlockParticles;

	UPROPERTY(EditAnywhere, Category = Combat)
		UParticleSystem* WeaponParticles;

		UPROPERTY(EditAnywhere, Category = Combat)
		USoundBase* ArrowSound;

		UPROPERTY(EditAnywhere, Category = Combat)
		UParticleSystem* ArrowParticles;

};