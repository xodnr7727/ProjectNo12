// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"
#include "Soul.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
}

void UAttributeComponent::ReceiveStun(float Damage)
{
	Stun = FMath::Clamp(Stun - Damage, 0.f, MaxStun);
}

void UAttributeComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost, 0.f, MaxStamina);
}

float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

float UAttributeComponent::GetStaminaPercent()
{
	return Stamina / MaxStamina;
}

float UAttributeComponent::GetExperiencePercent()
{
	return Experience / MaxExperience;
}

float UAttributeComponent::GetStunPercent()
{
	return Stun / MaxStun;
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UAttributeComponent::IsAlive()
{
	return Health > 0.f;
}

bool UAttributeComponent::IsStun()
{
	return Stun < 0.1f;
}

void UAttributeComponent::RecoveryStun()
{
	Stun += MaxStun;
}

void UAttributeComponent::RecoveryHealth()
{
	float RestoreAmount = MaxHealth * 0.3f;

	Health += RestoreAmount;

	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}

void UAttributeComponent::RegenMinusStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina - StaminaMinusRate * DeltaTime, 0.f, MaxStamina);
}

void UAttributeComponent::RegenHealth(float DeltaTime)
{
	Health = FMath::Clamp(Health + HealthRegenRate * DeltaTime, 0.f, MaxHealth);
}

void UAttributeComponent::AddEx(float DropEx)
{
	Experience = FMath::Clamp(Experience + DropEx, 0.f, MaxExperience);
}

void UAttributeComponent::AddSouls(int32 NumberOfSouls)
{
	Souls += NumberOfSouls;
	Experience = FMath::Clamp(Experience + Souls, 0.f, MaxExperience);
}


void UAttributeComponent::AddGold(int32 AmountOfGold)
{
	Gold += AmountOfGold;
}




