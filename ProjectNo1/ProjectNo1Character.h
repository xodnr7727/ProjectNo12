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
	void LevelUpAll();
	void LevelUpEC();
	void LevelUpES();
	void SavePlayerState();//플레이어 데이터 저장
	void LoadPlayerState();
	//플레이어 데이터 로드
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
		float TurnRateGamepad;

		UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnStunnedEnemyDetected OnStunnedEnemyDetected;

		UPROPERTY(BlueprintAssignable, Category = "Events")
		FOffStunnedEnemyDetected OffStunnedEnemyDetected;

protected:

	virtual void BeginPlay() override;

	/*기본 움직임*/
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Sprint(); //달리기
	void StopSprinting();
	void Dive(); //구르기
	virtual void DiveEnd() override;
	bool IsDiving();
	void EnableDive();//구르기 활성화
	void Parry(); //패리 기능
	void ParryCanDo(); //패리기능 사용 활성화
	void DrinkPotion(); //포션 마시기
	void DeactivatePotionEffect();
	void EnablePotion();//포션 활성화
	UFUNCTION(BlueprintCallable)
	void DrinkEnd();
	UFUNCTION(BlueprintCallable)
	void DrinkHealthPotion();
	bool HitReact();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	/*방패 막기 관련*/
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

	/*무기 장착 관련*/
	void EKeyPressed();//장비 장착
	void PlayEquip(const FName& SectionName);
	void EquipWeapon(AWeapon* Weapon);
	void Disarm(); //무장 해제
	void Arm(); //무장
	bool IsArm();
	bool CanDisarm();
	bool CanArm();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	/*공격 관련*/
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
	void AttackComboReset(); //콤보 리셋

	/*행동 부울 조건 */
	bool HasEnoughStamina();
	bool HasEnoughPotionStamina(); //포션 스태미너 유무
	bool HasEnoughAttackStamina(); //공격 스태미너 유무
	bool HasEnoughSkillStamina(); //스킬 스태미너 유무
	bool HasEnoughShieldStamina(); //막기 스태미너 유무
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Block")
	class UAnimMontage* ShieldMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parry")
	class UAnimMontage* ParryMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equip")
	class UAnimMontage* EquipUnEquipMontage;

	/*스킬 관련 */
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

	void SmallSkillPressed();
	UFUNCTION(BlueprintCallable)
	void EndSmallSkill();
	void EnableSmallSkill();
	UFUNCTION(BlueprintCallable)
	void ActivateSmallSkillEffect();
	UFUNCTION(BlueprintCallable)
	void DeactivateSmallSkillEffect();
	void ExecuteHold();//처형 홀드
	UFUNCTION(BlueprintCallable)
	void PerformBack();
	void CheckForStunnedEnemy(); //스턴 몬스터 체크
	void CheckForNotStunnedEnemy();

	void EnableHit(); //무적해제
	void ActivateSkillParticles();
	void ExecuteGetHit(FHitResult& HitResult);
	bool IsAttackSkill();
	void DeactivateSkillEffect(); //이펙트 해제
	void ReCounterDamage();
	void ReStunDamage();//데미지 복구
	void RestoreDamage(); //공격력 복구
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

	AActor* FindClosestEnemy();
private:

	    bool IsUnoccupied();
		bool Unequipoccupied();
		bool Dead();
		void SetHUDHealth();

		void SpawnDefaultWeapon(); //기본 무기 장착
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

		int32 CurrentComboStep; //콤보 단계
		TArray<FName> ComboSectionNames; //콤보 단계별 이름
		UPROPERTY(EditAnywhere, Category = "Skill")
		float AttackComboCount; //공격 콤보 초기화 변수
		FTimerHandle AttackComboCountdown; //콤보 초기화 타이머

		UPROPERTY(EditAnywhere, Category = "Skill")
		float ParryCountdown; // 패리기능 쿨타임 변수
		bool bCanParry;//패리기능 사용 가능 여부

		UPROPERTY(EditAnywhere, Category = "Skill")
		float PotionCooldown; // 포션 마시기 쿨타임 변수
		bool bCanDrinkPotion; // 포션 마실 수 있는지 여부

		UPROPERTY(EditAnywhere, Category = "Skill")
		float DiveCooldown; // 구르기 쿨타임 변수
		bool bCanDive; // 구르기 할 수 있는지 여부

		UPROPERTY(EditAnywhere, Category = "Skill")
		float RageCooldown; // 스킬 쿨타임 변수
		bool bCanRage; // 스킬 사용할 수 있는지 여부

		UPROPERTY(EditAnywhere, Category = "Skill")
		float LargeSkillCooldown; // 스킬 쿨타임 변수
		bool bCanLargeSkill; // 스킬 사용할 수 있는지 여부

		UPROPERTY(EditAnywhere, Category = "Skill")
		float SmallSkillCooldown; // 스킬 쿨타임 변수
		bool bCanSmallSkill; // 스킬 사용할 수 있는지 여부

		UPROPERTY(EditAnywhere, Category = "Skill")
		float SwordSkillCooldown; // 스킬 쿨타임 변수
		UPROPERTY(EditAnywhere, Category = "Skill")
		float SwordSkillDuration; // 스킬 지속 시간
		bool bCanSwordSkill; // 스킬 사용할 수 있는지 여부

		UPROPERTY(EditAnywhere, Category = "Skill")
		float WeaponSpellCooldown; // 스킬 쿨타임 변수
		UPROPERTY(EditAnywhere, Category = "Skill")
		float WeaponSpellDuration; // 스킬 지속 시간
		bool bCanWeaponSpell; // 스킬 사용할 수 있는지 여부
		bool AttackWeaponSpell; // 스킬 사용할 수 있는지 여부

		UPROPERTY(EditAnywhere, Category = "Attack Properties")
		float SpellDamage;

		UPROPERTY(EditAnywhere, Category = "Skill")
		float RageDuration;// 분노 지속 시간 (초)

		UPROPERTY(EditAnywhere, Category = "Skill")
		float PotionDuration;// 포션 지속 시간

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
		float GuardCounterCooldown; // 가드 카운터 쿨타임 변수
		UPROPERTY(EditAnywhere, Category = "Skill")
		float GuardCounterDisableTimer; // 가드 카운터 제한시간 타이머 변수
		bool bIsGuardCounterAttackEnabled; //가드 카운터 부울 변수
		bool bIsGuardCountdownEnabled; //가드 카운터 쿨타임 부울 변수

		UPROPERTY(EditAnywhere, Category = "Skill")
		float LevelUpCountdown;
		
		UPROPERTY(EditAnywhere, Category = "Skill")
		float EnableHitCountdown; //무적 시간 변수

		UPROPERTY(EditAnywhere, Category = "Skill")
		float DamagebackCountdown;

		UPROPERTY(EditAnywhere, Category = "Combat")
		float PushBackDistance;

		int32 RemainingMonsters;
public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }

};

