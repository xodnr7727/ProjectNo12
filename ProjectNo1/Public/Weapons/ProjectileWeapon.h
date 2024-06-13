// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ProjectileWeapon.generated.h"
class UProjectileMovementComponent;
class UBoxComponent;

UCLASS()
class PROJECTNO1_API AProjectileWeapon : public AItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileWeapon();

	void ProjectileEquip(USceneComponent* InParent, AActor* NewOwner, APawn* NewInstigator);

	virtual void Tick(float DeltaTime) override;

	void InitializeVelocity(FVector Direction, AActor* Target);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ExecuteGetHit(FHitResult& BoxHit);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float Damage;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MaxDistance;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent* SwordMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UParticleSystemComponent* ImpactEffect;

	UPROPERTY(VisibleAnywhere,Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, Category = "Skill Properties")
	class USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Skill Properties")
	class UParticleSystem* HitParticles;

	FVector InitialLocation;

	AActor* TargetActor;

public:

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SpearSpellSweepTrace();

	FORCEINLINE UBoxComponent* GetProjectileCollison() const { return CollisionComponent; }
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};
