#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "CharacterTypes.h"
#include "BaseCharacter.generated.h"
class AShield;
class APotion;
class AWeapon;
class AProjectileWeapon;
class AMySkillClass;
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
	void DirectionalHitFall(const FVector& ImpactPoint);

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
	void PlayWeaponSkillSound();
	void PlayDamageIncreaseSound();
	void PlayDamageIncreaseFailSound();
	void PlayRushSkillSound();

	void DisableCapsule();
	void EnableCapsule();

	virtual bool CanAttack();
	virtual bool CanLaserAttack();
	virtual bool CanRushAttack();
	virtual bool CanSmashAttack();
	virtual bool CanMagicAttack();
	virtual bool CanSwingAttack();
	virtual bool CanNeckSkill();

	UFUNCTION(BlueprintCallable)
	bool IsAlive();

	UFUNCTION(BlueprintCallable)
	bool IsStun();

	UFUNCTION(BlueprintCallable)
	bool NotEnoughStamina();

	bool IsAttacking();
	void DisableMeshCollision();

	bool HasEnoughShieldStamina();
	void PlayHitReactMontage(const FName& SectionName);
	void PlayStunReactMontage(const FName& SectionName);
	void PlayFallReactMontage(const FName& SectionName);
	void PlayBlockReactMontage();
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual int32 PlayStunMontage();
	virtual void PlayDiveMontage();
	virtual void PlayDrinkMontage();
	virtual void PlayNeckSkillMontage();
	virtual void PlayWeaponSpellSkillMontage();
	virtual void PlayLargeSkillMontage();
	virtual void PlayGuardCounterMontage();
	virtual void PlaySmallSkillMontage();
	virtual void PlaySwordSkillMontage();
	virtual void PlayLaserSkillMontage();
	virtual void PlayRushSkillMontage();
	virtual void PlaySmashSkillMontage();
	virtual void PlayMagicSkillMontage();
	virtual void PlaySwingSkillMontage();
	virtual void PlayTeleportSkillMontage();
	virtual void PlayPhaseEnterMontage();
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
	virtual void EndLaserSkill();

	UFUNCTION(BlueprintCallable)
	virtual void EndRushSkill();

	UFUNCTION(BlueprintCallable)
	virtual void EndSmashSkill();

	UFUNCTION(BlueprintCallable)
	virtual void EndSwingSkill();

	UFUNCTION(BlueprintCallable)
	virtual void EndEnterPhase();

	UFUNCTION(BlueprintCallable)
	virtual void EndMagicSkill();

	UFUNCTION(BlueprintCallable)
		void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
		void SetSkillCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
		void SetRushSkillCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
		void SetClawSkillCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
		void SetTeethSkillCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
		void SetShieldCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
		void SetLeftCastSkillCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapon)
		AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapon)
		AProjectileWeapon* EquippedProjectile;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Shield)
		AShield* EquippedShield;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Potion)
		APotion* EquippedPotion;

	UPROPERTY(VisibleAnywhere, Category = Enemy)
		ALichEnemy* LichEnemy;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	    AWeapon* EquippedNeck;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
		ATopArmor* EquippedArmor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	    AMySkillClass* MySkillClass;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
		AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = Combat)
		double WarpTargetDistance = 75.f;

	UPROPERTY(BlueprintReadOnly)
		TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
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
		UAnimMontage* FallReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* NeckSkillMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* LargeSkillMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* GuardCounterMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* SmallSkillMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* SwordSkillMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* LaserSkillMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* RushSkillMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* SmashSkillMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* MagicSkillMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* SwingSkillMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* TeleportSkillMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* PhaseEnterMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* DiveMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* DrinkMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		UAnimMontage* WeaponSpellSkillMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
		TArray<FName> AttackMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
		TArray<FName> DeathMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
		TArray<FName> StunMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
		TArray<FName> HitMontageSections;
	/*
	* Components
	*/
public:
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }
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
		USoundBase* WeaponSkillSound;

	UPROPERTY(EditAnywhere, Category = Combat)
		USoundBase* DamageIncreaseSound;

	UPROPERTY(EditAnywhere, Category = Combat)
		USoundBase* DamageIncreaseFailSound;

	UPROPERTY(EditAnywhere, Category = Combat)
		USoundBase* RushSkillSound;

	UPROPERTY(EditAnywhere, Category = Combat)
		USoundBase* ArrowSound;

	UPROPERTY(EditAnywhere, Category = Combat)
		UParticleSystem* ArrowParticles;
};