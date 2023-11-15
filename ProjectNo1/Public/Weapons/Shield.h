// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Characters/BaseCharacter.h"
#include "Shield.generated.h"
class USoundBase;
class UBoxComponent;
UCLASS()
class PROJECTNO1_API AShield : public AItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShield();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable)
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);

	void DeactivateEmbers();
	void DisableCapsuleCollision();
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	TArray<AActor*> IgnoreActors;

protected:

	UFUNCTION()
	void OnWeaponHit(AActor* OtherWeapon);//막기 이펙트 사운드 재생

	void OnParry(ALichEnemy* HitEnemy); //패리 이펙트 사운드 재생
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ExecuteGetHit(FHitResult& BoxHit); //막기 기능 일부

	void ExecuteGetStun(FHitResult& BoxHit); //패리 기능 일부

private:
	void BoxTrace(FHitResult& BoxHit);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Shield Properties")
	USoundBase* EquipSound;

	UPROPERTY(EditAnywhere, Category = "Shield Properties")
	USoundBase* BlockSound;

	UPROPERTY(EditAnywhere, Category = "Shield Properties")
	UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, Category = "Shield Properties")
	USoundBase* StunSound;

	UPROPERTY(EditAnywhere, Category = "Shield Properties")
	UParticleSystem* StunParticles;

public:
	FORCEINLINE UBoxComponent* GetShieldBox() const { return ShieldBox; }

	UPROPERTY(EditAnywhere, Category = "Shield Properties")
	UBoxComponent* ShieldBox;

};
