 // Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/BaseCharacter.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "ProjectNo1Character.generated.h"
class AItem;
class ASoul;
class ATreasure;
class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
class USlashOverlay;
class AMyPlayerController;
class UNiagaraSystem;
class USoundBase;
class UParticleSystemComponent;
UCLASS()
class AProjectNo1Character : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	AProjectNo1Character();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void GetBlock_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddEx(ASoul* Soul) override;
	void LevelUpAll();
	void LevelUpEC();
	void LevelUpES();
	virtual void AddGold(ATreasure* Treasure) override;
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
		float TurnRateGamepad;

protected:

	virtual void BeginPlay() override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);
	void EKeyPressed();//��� ����

	UFUNCTION(BlueprintCallable)
	void RMKeyPressed();

	UFUNCTION(BlueprintCallable)
	void RMKeyReleased();

	bool BlockCantState();
	void IfAttack();
	void Sprint(); //�޸���
	void StopSprinting(); 
	
	void EquipWeapon(AWeapon* Weapon);

	void EquipProjectileWeapon(AProjectileWeapon* ProjectileWeapon);

	virtual void AttackEnd() override;
	virtual void BlockEnd() override;
	void Parry(); //�и� ���
	void ParryCanDo(); //�и���� ��� Ȱ��ȭ
	virtual void DiveEnd() override;
	virtual void Attack() override;
	void Dive();
	bool IsDiving(); //������
	void EnableDive();
	void DrinkPotion(); //���� ���ñ�
	void DeactivatePotionEffect();
	void EnablePotion();
	void EnableRage();

	bool IsArm();

	bool CanDisarm();
	bool CanArm();
	bool CanBlock();
	bool HasEnoughStamina();
	bool HasEnoughPotionStamina();
	bool HasEnoughAttackStamina();
	bool HasEnoughSkillStamina();
	bool HasEnoughShieldStamina();
	bool IsOccupied();
	virtual bool CanAttack() override;
	virtual bool CanNeckSkill() override;
	void Disarm();
	void Arm();
	void DisBlock();
	void AsBlock();
	void PlayEquip(const FName& SectionName);
	virtual void Die() override;

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	bool HitReact();
	 
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	UFUNCTION(BlueprintCallable)
	void DrinkEnd();

	UFUNCTION(BlueprintCallable)
	void AttackComboResetCountDown();

	UFUNCTION(BlueprintCallable)
	void AttackComboStartCountDown();

	UFUNCTION(BlueprintCallable)
	void AttackComboReset(); //�޺� ����

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Block")
	class UAnimMontage* ShieldMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parry")
	class UAnimMontage* ParryMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equip")
	class UAnimMontage* EquipUnEquipMontage;

	UFUNCTION(BlueprintCallable)
	void EndAttacking();

	UFUNCTION(BlueprintCallable)
	void EndBlocking();

	UFUNCTION(BlueprintCallable)
	void EndLargeAttack();

	UFUNCTION(BlueprintCallable)
	void EndSwordSkill();

	UFUNCTION(BlueprintCallable)
	void EndSmallSkill();

	UFUNCTION(BlueprintCallable)
	void DrinkHealthPotion();

	UFUNCTION(BlueprintCallable)
	void DeactivateLargeSkillEffect();

	UFUNCTION(BlueprintCallable)
	void DeactivateSmallSkillEffect();

	UFUNCTION(BlueprintCallable)
	void ActivateLargeSkillEffect();

	UFUNCTION(BlueprintCallable)
	void ActivateSmallSkillEffect();

	UFUNCTION(BlueprintCallable)
	void DeactivateGuardCounterEffect();

	UFUNCTION(BlueprintCallable)
	void ActivateGuardCounterEffect();

	UFUNCTION(BlueprintCallable)
	void SwingSword();

	void ActivateSkillParticles();

	UFUNCTION(BlueprintCallable)
	void WeaponSpellLineTrace();
	void WeaponSpellAttack();
	void WeaponSpellSlashEffect();
	void ExecuteGetHit(FHitResult& HitResult);
	void EnableWeaponSpell();
	void PlayWeaponSpellHitSound(const FVector& ImpactPoint);

	UFUNCTION(BlueprintCallable)
	void DeactivateWeaponSpellEffect();

	UFUNCTION(BlueprintCallable)
	void ActivateWeaponSpellEffect();

	void OnNeckSkillPressed();
	void LargeSkillPressed();
	void GuardCounterPressed();
	void ReCounterDamage();
	void EnableGuardCountdown();
	void EnableGuardCounter();
	void DisableGuardCounter();
	void EnableSwordSkill();
	void EnableLargeSkill();
	void EnableSmallSkill();
	void EnableHit();
	bool IsAttackSkill();
	void SmallSkillPressed();
	void ReStunDamage();
	void OnSwordSkillPressed();
	void DeactivateSkillEffect(); //����Ʈ ����
	void RestoreDamage(); //���ݷ� ����

	void IncreaseSpellDamage();
	void RestoreSpellDamage();

	void EquipNeck(AWeapon* NewNeck);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	UFUNCTION(BlueprintImplementableEvent)
		void CreateFields(const FVector& FieldLocation);
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Function to check if there is a stunned enemy in front of the player
	UFUNCTION(BlueprintPure, Category = "SpecialTargeting")
	bool HasStunnedEnemyInFront();

	// Function to enable special targeting attack when a stunned enemy is in front
	UFUNCTION(BlueprintCallable, Category = "SpecialTargeting")
	void EnableSpecialTargetingAttack();

	UFUNCTION(BlueprintCallable, Category = "SpecialTargeting")
	void DisableSpecialTargetingAttack();

	// Function to handle the input for the special targeting attack
	void SpecialTargetingAttackInput();

	void InitializeSlashOverlay();
private:

	    bool IsUnoccupied();
		bool Unequipoccupied();
		bool Dead();
		void SetHUDHealth();

		void SpawnDefaultWeapon();
		void SpawnDefaultWeaponTwo();
		void SpawnDefaultPotionOne();

		UPROPERTY(VisibleInstanceOnly)
		AItem* OverlappingItem;

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

		/** Follow camera */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

		UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

		UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		EActionState ActionState = EActionState::EAS_Unoccupied;

		UPROPERTY()
		USlashOverlay* SlashOverlay;

		UPROPERTY()
		AMyPlayerController* MyPlayerController;

		UPROPERTY(VisibleAnywhere)
		USceneComponent* BoxTraceStart;

		UPROPERTY(VisibleAnywhere)
		USceneComponent* BoxTraceEnd;

		UPROPERTY(EditAnywhere)
		TSubclassOf<class AWeapon> WeaponClass;

		UPROPERTY(EditAnywhere)
		TSubclassOf<class AShield> ShieldClass;

		UPROPERTY(EditAnywhere)
		TSubclassOf<class APotion> PotionClass;

		UPROPERTY(EditAnywhere)
		TSubclassOf<class AProjectileWeapon> ProjectileWeaponClass;

		UPROPERTY(EditAnywhere)
		TSubclassOf<class AMySkillClass> LargeSkillClass;

		UPROPERTY(EditAnywhere, Category = "Combat")
		TSubclassOf<class AProjectileWeapon> SwordProjectileClass;

		int32 CurrentComboStep; //�޺� �ܰ�
		TArray<FName> ComboSectionNames; //�޺� �ܰ躰 �̸�
		float AttackComboCount; //���� �޺� �ʱ�ȭ ����
		FTimerHandle AttackComboCountdown; //�޺� �ʱ�ȭ Ÿ�̸�

		float ParryCountdown; // �и���� ��Ÿ�� ����
		bool bCanParry;//�и���� ��� ���� ����

		UPROPERTY(EditAnywhere, Category = "Skill")
		float PotionCooldown; // ���� ���ñ� ��Ÿ�� ����
		bool bCanDrinkPotion; // ���� ���� �� �ִ��� ����

		UPROPERTY(EditAnywhere, Category = "Skill")
		float DiveCooldown; // ������ ��Ÿ�� ����
		bool bCanDive; // ������ �� �� �ִ��� ����

		UPROPERTY(EditAnywhere, Category = "Skill")
		float RageCooldown; // ��ų ��Ÿ�� ����
		bool bCanRage; // ��ų ����� �� �ִ��� ����

		UPROPERTY(EditAnywhere, Category = "Skill")
		float LargeSkillCooldown; // ��ų ��Ÿ�� ����
		bool bCanLargeSkill; // ��ų ����� �� �ִ��� ����

		UPROPERTY(EditAnywhere, Category = "Skill")
		float SmallSkillCooldown; // ��ų ��Ÿ�� ����
		bool bCanSmallSkill; // ��ų ����� �� �ִ��� ����

		UPROPERTY(EditAnywhere, Category = "Skill")
		float SwordSkillCooldown; // ��ų ��Ÿ�� ����
		UPROPERTY(EditAnywhere, Category = "Skill")
		float SwordSkillDuration; // ��ų ���� �ð�
		bool bCanSwordSkill; // ��ų ����� �� �ִ��� ����

		UPROPERTY(EditAnywhere, Category = "Skill")
		float WeaponSpellCooldown; // ��ų ��Ÿ�� ����
		UPROPERTY(EditAnywhere, Category = "Skill")
		float WeaponSpellDuration; // ��ų ���� �ð�
		bool bCanWeaponSpell; // ��ų ����� �� �ִ��� ����
		bool AttackWeaponSpell; // ��ų ����� �� �ִ��� ����

		UPROPERTY(EditAnywhere, Category = "Attack Properties")
		float SpellDamage;

		UPROPERTY(EditAnywhere, Category = "Skill")
		float RageDuration;// �г� ���� �ð� (��)

		UPROPERTY(EditAnywhere, Category = "Skill")
		float PotionDuration;// ���� ���� �ð�

		UPROPERTY(VisibleAnywhere, Category = "Skill")
		UParticleSystemComponent* RageSkillEffect;

		UPROPERTY(VisibleAnywhere, Category = "Skill")
		UParticleSystemComponent* PotionSkillEffect;

		UPROPERTY(EditAnywhere, Category = "Skill")
		UParticleSystem* SkillParticles;

		UPROPERTY(EditAnywhere, Category = "Skill")
		UNiagaraSystem* ImpactEffect;

		UPROPERTY(EditAnywhere, Category = "Skill")
		UNiagaraSystem* SpellEffectA;

		UPROPERTY(EditAnywhere, Category = "Skill")
		UNiagaraSystem* SpellEffectB;

		UPROPERTY(EditAnywhere, Category = "Skill")
		UNiagaraSystem* SpellEffectC;

		UPROPERTY(EditAnywhere, Category = "Skill")
		UParticleSystem* EndPointEffect;

		UPROPERTY(EditAnywhere, Category = "Skill")
		USoundBase* WeaponSpellSound;
		 
		// Range to detect stunned enemies in front of the player
		UPROPERTY(EditDefaultsOnly, Category = "SpecialTargeting")
		float SpecialTargetingRange;

		// Flag to track if the special targeting attack is enabled
		bool bIsSpecialTargetingEnabled;

		UPROPERTY(EditAnywhere, Category = "Skill")
		float GuardCounterCooldown; // ���� ī���� ��Ÿ�� ����
		UPROPERTY(EditAnywhere, Category = "Skill")
		float GuardCounterDisableTimer; // ���� ī���� ���ѽð� Ÿ�̸� ����
		bool bIsGuardCounterAttackEnabled; //���� ī���� �ο� ����
		bool bIsGuardCountdownEnabled; //���� ī���� ��Ÿ�� �ο� ����

		UPROPERTY(EditAnywhere, Category = "Skill")
		float LevelUpCountdown;
		
		UPROPERTY(EditAnywhere, Category = "Skill")
		float EnableHitCountdown; //���� �ð� ����

		UPROPERTY(EditAnywhere, Category = "Skill")
		float DamagebackCountdown;
public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
};

