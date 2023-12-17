// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "ProjectileWeapon.generated.h"
class UBoxComponent;
class UCapsuleComponent;
class UParticleSystemComponent;
class UProjectileMovementComponent;
UCLASS()
class PROJECTNO1_API AProjectileWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileWeapon();

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float Damage;

	UPROPERTY(EditAnywhere, Category = "Skill Properties")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Skill Properties")
	UParticleSystem* HitParticles;
public:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* SwordMesh;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	UCapsuleComponent* CollisionComponent;

	// 몬스터에게 타격을 입히는 함수 선언
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
