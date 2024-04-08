// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"
#include "Soul.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

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

bool UAttributeComponent::IsCanLaserSkill()
{
	return Health < 0.51f;
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
	HealthRegenRate = 12.f;
	/*float RestoreAmount = MaxHealth * 0.3f;

	Health += RestoreAmount;

	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}*/
}

void UAttributeComponent::BaseHealth()
{
	HealthRegenRate = 2.f;
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
	if (Experience >= MaxExperience) {
		Experience = 0.0f;
		Level += 1;
		MaxHealth += 10;
		MaxExperience += 50;
		MaxStamina += 10;
		HealthRegenRate += 1;
		StaminaRegenRate += 2;
		//최대 체력, 경험치, 스태미너 증가 추가 and 리젠량.
		AProjectNo1Character* ProjectNo1Character = Cast<AProjectNo1Character>(GetOwner());
		if (ProjectNo1Character)
		{
			FVector SpawnLocation = ProjectNo1Character->GetActorLocation();
			//UParticleSystemComponent* LevelUpEffect = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LevelUpParticle, SpawnLocation, FRotator::ZeroRotator, true);
			if (LevelUpParticle && GetWorld()) {
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					LevelUpParticle,
					SpawnLocation
				); //레벨업 이펙트, 사운드
			}
		}
	}
}

void UAttributeComponent::AddLevel(int32 AmountOfLevel)
{
	if (Experience >= MaxExperience) {
		Experience = 0.0f;
		Level += 1;
	}
}


void UAttributeComponent::AddGold(int32 AmountOfGold)
{
	Gold += AmountOfGold;
}




