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
	void EKeyPressed();//��� ����

	UFUNCTION(BlueprintCallable)
	void RMKeyPressed();

	UFUNCTION(BlueprintCallable)
	void RMKeyReleased();

	void IfAttack();
	void Sprint(); //�޸���
	void StopSprinting(); 
	
	void EquipWeapon(AWeapon* Weapon);

	virtual void AttackEnd() override;
	virtual void BlockEnd() override;
	void Parry(); //�и� ���
	void ParryCanDo(); //�и���� ��� Ȱ��ȭ
	virtual void DiveEnd() override;
	virtual void Attack() override;
	void AttackComboReset(); //�޺� ����
	void Dive();//������
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
	void DeactivateSkillEffect(); //����Ʈ ����
	void RestoreDamage(); //���ݷ� ����

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

		int32 CurrentComboStep; //�޺� �ܰ�
		TArray<FName> ComboSectionNames; //�޺� �ܰ躰 �̸�
		float AttackComboCount; //���� �޺� �ʱ�ȭ ����

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
		float RageDuration;// �г� ���� �ð� (��)

		UPROPERTY(EditAnywhere, Category = "Skill")
		float PotionDuration;// ���� ���� �ð�

		UPROPERTY(VisibleAnywhere, Category = "Skill")
		UParticleSystemComponent* RageSkillEffect;

		UPROPERTY(VisibleAnywhere, Category = "Skill")
		UParticleSystemComponent* PotionSkillEffect;

public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
};

