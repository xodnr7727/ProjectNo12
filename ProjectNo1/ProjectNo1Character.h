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
class UInventoryUI;
class AMyPlayerController;
class UNiagaraSystem;
class USoundBase;
class UParticleSystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStunnedEnemyDetected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOffStunnedEnemyDetected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyBackDetected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOffEnemyBackDetected);

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
	virtual void AddGold(ATreasure* Treasure) override;
	void ICDamageGold(); //������ ���� ��� �Ҹ�
	void ICAmorGold(); //�Ƹ� ���� ��� �Ҹ�
	void IncreaseDamage();
	void SetStatus(); //����â ����ȭ
	void SetStatusWithDmgAm();
	void LevelUpAll();
	void LevelUpEC();
	void LevelUpES();
	void SavePlayerState();//�÷��̾� ������ ����
	void LoadPlayerState();
	void MapLoadRegionOpen();
	//�÷��̾� ������ �ε�

	void Respawn(); // ĳ���� ���� �� ������
	void PlayerStartUI();

	void ShowSlashOverlay();

	void HideSlashOverlay();

	/*���� ���� ����*/
	void CaveRegionOpen();
	void IceLandRegionOpen();
	void ForestRegionOpen();

	UFUNCTION()
	void PlayerCanMove(); //ĳ���� ������ Ȱ��ȭ

	UFUNCTION()
	void OpenInfoWidget(); //�÷��̾� �ɷ�ġ â ����

	UFUNCTION()
	void OpenMapWidget(); //���� ����

	UFUNCTION()
	void SystemWidget();

	UFUNCTION()
	void OpenSystemWidget();

	UFUNCTION()
	void OpenDamageIncreaseWidget(); //���� ����

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
		float TurnRateGamepad;

		UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnStunnedEnemyDetected OnStunnedEnemyDetected;

		UPROPERTY(BlueprintAssignable, Category = "Events")
		FOffStunnedEnemyDetected OffStunnedEnemyDetected;

		UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnEnemyBackDetected OnEnemyBackDetected;

		UPROPERTY(BlueprintAssignable, Category = "Events")
		FOffEnemyBackDetected OffEnemyBackDetected;

		/*�ູ ��ȣ �ۿ� ����*/
		void ShowInteractMessage();
		void HideInteractMessage();
		void BlessingPointSetting();
		void BlessingPointCaveSetting();
		void BlessingPointForestSetting();
		void BlessingPointIceLandSetting();
		void BlessingPointInteract();
		bool IsBlessingPointInteract();
		bool bInRangePoint();

		UFUNCTION(BlueprintCallable)
		void EndInteract();

		void bInRangePointTrue();
		void bInRangePointFalse();

protected:

	virtual void BeginPlay() override;

	/*�⺻ ������*/
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Sprint(); //�޸���
	void StopSprinting();
	void Dive(); //������
	virtual void DiveEnd() override;
	bool IsDiving();
	void EnableDive();//������ Ȱ��ȭ
	void Parry(); //�и� ���
	void ParryCanDo(); //�и���� ��� Ȱ��ȭ
	void DrinkPotion(); //���� ���ñ�
	void DeactivatePotionEffect();
	void EnablePotion();//���� Ȱ��ȭ
	UFUNCTION(BlueprintCallable)
	void DrinkEnd();
	UFUNCTION(BlueprintCallable)
	void DrinkHealthPotion();
	bool HitReact();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	/*���� ���� ����*/
	void DisBlock();
	void AsBlock();
	UFUNCTION(BlueprintCallable)
	void RMKeyPressed();
	UFUNCTION(BlueprintCallable)
	void RMKeyReleased();
	UFUNCTION(BlueprintCallable)
	void EndBlocking();
	bool CanBlock();
	bool BlockCantState();
	virtual void BlockEnd() override;
	void IfAttack();

	/*���� ���� ����*/
	void EKeyPressed();//��� ����
	void PlayEquip(const FName& SectionName);
	void EquipWeapon(AWeapon* Weapon);
	void Disarm(); //���� ����
	void Arm(); //����
	bool IsArm();
	bool CanDisarm();
	bool CanArm();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	/*���� ����*/
	virtual void Attack() override;
	virtual void AttackEnd() override;
	virtual bool CanAttack() override;
	UFUNCTION(BlueprintCallable)
	void EndAttacking();
	UFUNCTION(BlueprintCallable)
	void AttackComboResetCountDown();

	UFUNCTION(BlueprintCallable)
	void AttackComboStartCountDown();

	UFUNCTION(BlueprintCallable)
	void AttackComboReset(); //�޺� ����

	UFUNCTION(BlueprintCallable)
	void AttackSweepTrace();

	/*�ൿ �ο� ���� */
	bool HasEnoughStamina();
	bool HasEnoughPotionStamina(); //���� ���¹̳� ����
	bool HasEnoughAttackStamina(); //���� ���¹̳� ����
	bool HasEnoughSkillStamina(); //��ų ���¹̳� ����
	bool HasEnoughShieldStamina(); //���� ���¹̳� ����
	bool IsOccupied();
	virtual bool CanNeckSkill() override;

	virtual void Die() override;
	void PlayerDieUI();
	
	UFUNCTION()
	void CheckBossMonsters();

	UFUNCTION()
	void GameClearUI();

	UFUNCTION()
	void RemoveClearWidget();

	UFUNCTION()
	void ToggleAllMenuUI();

	UFUNCTION()
	void AllMenuWidget();

	UFUNCTION()
	void InfoWidget();

	UFUNCTION()
	void DamageIncreaseWidget();

	UFUNCTION()
	void MapWidget();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Block")
	class UAnimMontage* ShieldMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parry")
	class UAnimMontage* ParryMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equip")
	class UAnimMontage* EquipUnEquipMontage;

	/*��ų ���� */
	UFUNCTION(BlueprintCallable)
	void SwingSword();

	UFUNCTION(BlueprintCallable)
	void WeaponSpellLineTrace();
	void WeaponSpellAttack();
	void WeaponSpellSlashEffect();
	void EnableWeaponSpell();
	void PlayWeaponSpellHitSound(const FVector& ImpactPoint);
	UFUNCTION(BlueprintCallable)
	void DeactivateWeaponSpellEffect();
	UFUNCTION(BlueprintCallable)
	void ActivateWeaponSpellEffect();
	
	void OnNeckSkillPressed();
	void EnableRage();

	void LargeSkillPressed();
	void EnableLargeSkill();
	UFUNCTION(BlueprintCallable)
	void EndLargeAttack();
	UFUNCTION(BlueprintCallable)
	void DeactivateLargeSkillEffect();
	UFUNCTION(BlueprintCallable)
	void ActivateLargeSkillEffect();

	/*���� ī����*/
	void GuardCounterPressed();
	void EnableGuardCountdown();
	void EnableGuardCounter();
	void DisableGuardCounter();
	UFUNCTION(BlueprintCallable)
	void ActivateGuardCounterEffect();
	UFUNCTION(BlueprintCallable)
	void DeactivateGuardCounterEffect();


	void OnSwordSkillPressed();
	void EnableSwordSkill();
	UFUNCTION(BlueprintCallable)
	void EndSwordSkill();

	/*���� ����*/
	void SmallSkillPressed();
	UFUNCTION(BlueprintCallable)
	void EndSmallSkill();
	void EnableSmallSkill();
	UFUNCTION(BlueprintCallable)
	void ActivateSmallSkillEffect();
	UFUNCTION(BlueprintCallable)
	void DeactivateSmallSkillEffect();
	void ExecuteHold();//ó�� Ȧ��
	UFUNCTION(BlueprintCallable)
	void PerformBack();
	void CheckForStunnedEnemy(); //���� ���� üũ
	void CheckForNotStunnedEnemy();
	void CheckForBackEnemy(); //���� ���� üũ
	void CheckForNotBackEnemy();
	UFUNCTION(BlueprintPure, Category = "SpecialTargeting")
	bool HasEnemyBackInFront();
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

	/*�ູ ��ȣ �ۿ� ����*/
	void BlessingPointInteractInput();

	void EnableHit(); //��������
	void ActivateSkillParticles();
	void ExecuteGetHit(FHitResult& HitResult);
	bool IsAttackSkill();
	void DeactivateSkillEffect(); //����Ʈ ����
	void RestoreAllDamage();
	void RestoreDamage(); //���ݷ� ����
	void IncreaseSkillDamage();
	void RestoreSkillDamage();
	void IncreaseStunDamage();
	void RestoreStunDamage();
	void IncreaseCounterDamage();
	void RestoreCounterDamage();
	void IncreaseSpellDamage();
	void RestoreSpellDamage();

	void EquipNeck(AWeapon* NewNeck);
	void EquipProjectileWeapon(AProjectileWeapon* ProjectileWeapon);
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

	void InitializeSlashOverlay();

	AActor* FindClosestEnemy();

	UPROPERTY(EditAnywhere, Category = "UI")
	bool bIsAllMenuUIVisible;
private:

	    bool IsUnoccupied();
		bool Unequipoccupied();
		bool Dead();
		void SetHUDHealth();

		void SpawnDefaultWeapon(); //�⺻ ���� ����
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

		UPROPERTY(EditDefaultsOnly, Category = "UI")
		TSubclassOf<UInventoryUI> InventoryUIClass;

		UPROPERTY()
		UInventoryUI* InventoryUI;

		UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UUserWidget> ClearWidgetClass;

		class UUserWidget* ClearWidget;

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
		UPROPERTY(EditAnywhere, Category = "Skill")
		float AttackComboCount; //���� �޺� �ʱ�ȭ ����
		FTimerHandle AttackComboCountdown; //�޺� �ʱ�ȭ Ÿ�̸�

		UPROPERTY(EditAnywhere, Category = "Skill")
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
		float WeaponSpellCountdown; // ��ų ��Ÿ�� ����
		bool bCanWeaponSpell; // ��ų ����� �� �ִ��� ����
		bool AttackWeaponSpell; // ��ų ����� �� �ִ��� ����

		UPROPERTY(EditAnywhere, Category = "Skill")
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

		UPROPERTY(EditAnywhere, Category = "Combat")
		float PushBackDistance;

		int32 RemainingMonsters;

		UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UDamageIncreaseWidget> DamageIncreaseWidgetClass;

		UPROPERTY()
		UDamageIncreaseWidget* DamageIncreaseWidgetInstance;

		UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UAllMenuWidget> AllMenuWidgetClass;

		UPROPERTY()
		UAllMenuWidget* AllMenuWidgetInstance;

		UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UMapWidget> MapWidgetClass;

		UPROPERTY()
		UMapWidget* MapWidgetInstance;

		UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class UInfoWidget> InfoWidgetClass;

		UPROPERTY()
		UInfoWidget* InfoWidgetInstance;

		UPROPERTY(EditAnywhere, Category = "UI")
		TSubclassOf<class USystemWidget> SystemWidgetClass;

		UPROPERTY()
		USystemWidget* SystemWidgetInstance;

		UPROPERTY(EditAnywhere, Category = "Combat")
		bool bPlayerDead;

		UPROPERTY(EditAnywhere, Category = "Combat")
		float Amor;

		UPROPERTY(EditAnywhere, Category = "Combat")
		float WeaponDamage;

		UPROPERTY(EditAnywhere, Category = "Combat")
		bool bDamageIncreaseState;

		UPROPERTY()
		ABlessingPoint* LastBlessingPoint;
		bool bIsBlessingPointInteractEnabled;
		bool bInRange;

		UPROPERTY()
		FVector BlessingPoint;

		UPROPERTY(EditAnywhere, Category = "Map")
		bool bCaveState;

		UPROPERTY(EditAnywhere, Category = "Map")
		bool bIceLandState;

		UPROPERTY(EditAnywhere, Category = "Map")
		bool bForestState;
public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	FORCEINLINE float GetAmor() const { return Amor; }
	FORCEINLINE float GetWeaponDamage() const { return WeaponDamage; }

};

