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
class USphereComponent;
UCLASS()
class PROJECTNO1_API AProjectileWeapon : public AItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileWeapon();

	void ThrowInDirection(const FVector& ShootDirection);

	void ProjectileEquip(USceneComponent* InParent, AActor* NewOwner, APawn* NewInstigator);

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ExecuteBallHit(FHitResult& BoxHit);

	void DisableCapsuleCollision();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float Damage;

	UPROPERTY(EditAnywhere, Category = "Skill Properties")
	class USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Skill Properties")
	class UParticleSystem* HitParticles;

public:
	UPROPERTY(VisibleAnywhere, Category = "State")
	float MyInitialSpeed;

	UPROPERTY(VisibleAnywhere, Category = "State")
	float MyMaxSpeed;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent* SwordMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UParticleSystemComponent* ImpactEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 몬스터에게 타격을 입히는 함수 선언
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FORCEINLINE class USphereComponent* GetProjectileCollison() const { return CollisionComponent; }

	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};
