// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Soul.h"
#include "AttributeComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTNO1_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RegenHealth(float DeltaTime);
	void RegenStamina(float DeltaTime);
	void RegenMinusStamina(float DeltaTime);
	void RecoveryStun();
	void RecoveryHealth();

protected:
	virtual void BeginPlay() override;


private:
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float Health;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float Stamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float Experience;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float MaxExperience;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float Stun;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float MaxStun;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float AttackCost = 10.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float SkillCost = 15.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float DiveCost = 14.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float ShieldCost = 30.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float HealthRegenRate = 4.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float StaminaRegenRate = 20.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float StaminaMinusRate = 10.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float StunRegenRate = 200.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		int32 Gold;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		int32 Souls;

public:
	void ReceiveDamage(float Damage);
	void ReceiveStun(float Damage);
	float GetHealthPercent();
	float GetStunPercent();
	bool IsAlive();
	bool IsStun();

	void UseStamina(float StaminaCost);
	float GetStaminaPercent();
	float GetExperiencePercent();
	FORCEINLINE float GetAttackCost() const { return AttackCost; }
	FORCEINLINE float GetSkillCost() const { return SkillCost; }
	FORCEINLINE float GetDiveCost() const { return DiveCost; }
	FORCEINLINE float GetShieldCost() const { return ShieldCost; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	void AddSouls(int32 NumberOfSouls);
	void AddGold(int32 AmountOfGold);
	void AddEx(float DropEx);
	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE float GetExperience() const { return Experience; }
	FORCEINLINE int32 GetSouls() const { return Souls; }

};
