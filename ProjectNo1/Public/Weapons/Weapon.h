// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Characters/BaseCharacter.h"
#include "Weapons/ProjectileWeapon.h"
#include "Weapon.generated.h"
class USoundBase;
class UBoxComponent;
UCLASS()
class PROJECTNO1_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	UFUNCTION(BlueprintCallable)
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);

	void SpawnLaserSkillHitParticles(const FVector& ImpactPoint);
	void PlayLaserSkillSound();
	void SpawnRushSkillHitParticles(const FVector& ImpactPoint);
	void PlayRushSkillSound();

	void SpawnSkillHitParticles(const FVector& ImpactPoint);
	void PlaySkillSound();

	void DeactivateLaserSkillEffect();
	void ActivateLaserSkillEffect();

	void DeactivateGuardCounterEffect();
	void ActivateGuardCounterEffect();

	void DeactivateLargeSkillEffect();
	void ActivateLargeSkillEffect();

	void DeactivateLeftCastSkillEffect();
	void ActivateLeftCastSkillEffect();

	void DeactivateWeaponSpellEffect();
	void ActivateWeaponSpellEffect();

	void DeactivateLichSwingAttackEffect();
	void ActivateLichSwingAttackEffect();

	void DeactivateLichSmashAttackEffect();
	void ActivateLichSmashAttackEffect();

	void DeactivateSmallSkillEffect();
	void ActivateSmallSkillEffect();

	void DeactivateEmbers();
	virtual void DisableCapsuleCollision();
	virtual void PlayEquipSound();

	void IncreaseDamage();
	void RestoreDamage();
	void IncreaseSkillDamage();
	void RestoreSkillDamage();
	void IncreaseStunDamage();
	void RestoreStunDamage();
	void IncreaseCounterDamage();
	void RestoreCounterDamage();

	void IncreaseLaserSkillDamage();
	void RestoreLaserSkillDamage();

	void GoldIncreaseDamage();
	void GoldIncreaseAmor();

	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	TArray<AActor*> IgnoreActors;

protected:
	virtual void BeginPlay() override;


	/*
	virtual void OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
*/
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnSkillBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnLaserSkillBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRushSkillBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool CanShieldBlock();

	void ExecuteGetHit(FHitResult& BoxHit);

	void ExecuteGetBlock(FHitResult& BoxHit);

	UFUNCTION(BlueprintImplementableEvent)
		void CreateFields(const FVector& FieldLocation);

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AProjectileWeapon> ProjectileWeaponClass;

private:

	void BoxTrace(FHitResult& BoxHit);

	UPROPERTY(VisibleAnywhere)
	    USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	    USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		FVector BoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		bool bShowBoxDebug = false;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	    USoundBase* EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	    UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	     UBoxComponent* SkillBox;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		 UBoxComponent* LaserSkillBox;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		 UBoxComponent* RushSkillBox;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		 UBoxComponent* ClawSkillBox;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		 UBoxComponent* TeethSkillBox;
   
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		 UBoxComponent* LeftCastSkillBox;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	    USkeletalMeshComponent* SwordMesh;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		float Damage;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		USoundBase* BlockSound;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		UParticleSystem* HitParticles;

	EWeaponAState WeaponAState = EWeaponAState::EIS_NoAttaking;

	UPROPERTY(EditAnywhere, Category = "Skill")
		USoundBase* SkillSound;

	UPROPERTY(EditAnywhere, Category = "Skill")
		UParticleSystem* SkillHitParticles;

	UPROPERTY(EditAnywhere, Category = "Skill")
		USoundBase* LaserSkillSound;

	UPROPERTY(EditAnywhere, Category = "Skill")
		UParticleSystem* LaserSkillHitParticles;

	UPROPERTY(EditAnywhere, Category = "Skill")
		USoundBase* RushSkillSound;

	UPROPERTY(EditAnywhere, Category = "Skill")
		UParticleSystem* RushSkillHitParticles;

	UPROPERTY(VisibleAnywhere, Category = "Skill")
		UParticleSystemComponent* LargeSkillEffect;

	UPROPERTY(VisibleAnywhere, Category = "Skill")
		UParticleSystemComponent* SmallSkillEffect;

	UPROPERTY(VisibleAnywhere, Category = "Skill")
		UParticleSystemComponent* GuardCounterEffect;

	UPROPERTY(VisibleAnywhere, Category = "Skill")
	    UParticleSystemComponent* LaserSkillEffect;

	UPROPERTY(VisibleAnywhere, Category = "Skill")
	   UNiagaraComponent* LeftCastEffect;

	UPROPERTY(EditAnywhere, Category = "Skill")
		USoundBase* CastSkillSound;

	UPROPERTY(VisibleAnywhere, Category = "Skill")
		UNiagaraComponent* WeaponSpellAttackEffect;

	UPROPERTY(VisibleAnywhere, Category = "Skill")
		UNiagaraComponent* LichSwingAttackEffect;

	UPROPERTY(VisibleAnywhere, Category = "Skill")
		UNiagaraComponent* LichSmashAttackEffect;

public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
	FORCEINLINE UBoxComponent* GetSkillBox() const { return SkillBox; }
	FORCEINLINE UBoxComponent* GetLaserSkillBox() const { return LaserSkillBox; }
	FORCEINLINE UBoxComponent* GetRushSkillBox() const { return RushSkillBox; }
	FORCEINLINE UBoxComponent* GetClawSkillBox() const { return ClawSkillBox; }
	FORCEINLINE UBoxComponent* GetTeethSkillBox() const { return TeethSkillBox; }
	FORCEINLINE UBoxComponent* GetLeftCastSkillBox() const { return LeftCastSkillBox; }	
	FORCEINLINE float GetDamage() const { return Damage; }
};
