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
	void EKeyPressed();//장비 장착

	UFUNCTION(BlueprintCallable)
	void RMKeyPressed();

	UFUNCTION(BlueprintCallable)
	void RMKeyReleased();

	void IfAttack();
	void Sprint(); //달리기
	void StopSprinting(); 
	
	void EquipWeapon(AWeapon* Weapon);

	virtual void AttackEnd() override;
	virtual void BlockEnd() override;
	void Parry(); //패리 기능
	void ParryCanDo(); //패리기능 사용 활성화
	virtual void DiveEnd() override;
	virtual void Attack() override;
	void AttackComboReset(); //콤보 리셋
	void Dive();//구르기
	void EnableDive();
	void DrinkPotion(); //포션 마시기
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

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
	 
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

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
	void DrinkHealthPotion();

	UFUNCTION(BlueprintCallable)
	void DeactivateLargeSkillEffect();

	UFUNCTION(BlueprintCallable)
	void DeactivateSmallSkillEffect();

	UFUNCTION(BlueprintCallable)
	void ActivateLargeSkillEffect();

	UFUNCTION(BlueprintCallable)
	void ActivateSmallSkillEffect();

	void OnNeckSkillPressed();
	void LargeSkillPressed();
	void EnableLargeSkill();
	void EnableSmallSkill();
	bool IsAttackSkill();
	void SmallSkillPressed();
	void OnSwordSkillPressed();
	void DeactivateSkillEffect(); //이펙트 해제
	void RestoreDamage(); //공격력 복구

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

private:

	    void InitializeSlashOverlay();
	    bool IsUnoccupied();
		bool Unequipoccupied();
		void SetHUDHealth();

		void SpawnDefaultWeapon();
		void SpawnDefaultWeaponTwo();

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

		UPROPERTY(VisibleAnywhere)
		USceneComponent* BoxTraceStart;

		UPROPERTY(VisibleAnywhere)
		USceneComponent* BoxTraceEnd;

		UPROPERTY(EditAnywhere)
		TSubclassOf<class AWeapon> WeaponClass;

		UPROPERTY(EditAnywhere)
		TSubclassOf<class AShield> ShieldClass;

		UPROPERTY(EditAnywhere)
		TSubclassOf<class AProjectileWeapon> ProjectileWeaponClass;

		UPROPERTY(EditAnywhere)
		TSubclassOf<class AMySkillClass> LargeSkillClass;

		int32 CurrentComboStep; //콤보 단계
		TArray<FName> ComboSectionNames; //콤보 단계별 이름
		float AttackComboCount; //공격 콤보 초기화 변수

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
		float RageDuration;// 분노 지속 시간 (초)

		UPROPERTY(EditAnywhere, Category = "Skill")
		float PotionDuration;// 포션 지속 시간

		UPROPERTY(VisibleAnywhere, Category = "Skill")
		UParticleSystemComponent* RageSkillEffect;

		UPROPERTY(VisibleAnywhere, Category = "Skill")
		UParticleSystemComponent* PotionSkillEffect;

public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
};

