// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/Actor.h"
#include "TopArmor.generated.h"
class USoundBase;
class UBoxComponent;
UCLASS()
class PROJECTNO1_API ATopArmor : public AItem
{
	GENERATED_BODY()

		public:
		// Sets default values for this actor's properties
	ATopArmor();

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
		void OnWeaponHit(AActor* OtherWeapon);


		UFUNCTION()
		void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		void ExecuteGetHit(FHitResult& BoxHit);

	private:

		UPROPERTY(EditAnywhere, Category = "Shield Properties")
		USoundBase* EquipSound;

		UPROPERTY(EditAnywhere, Category = "Shield Properties")
		USoundBase* BlockSound;

		UPROPERTY(EditAnywhere, Category = "Shield Properties")
		UParticleSystem* HitParticles;

	public:
		FORCEINLINE UBoxComponent* GetShieldBox() const { return ShieldBox; }

		UPROPERTY(VisibleAnywhere, Category = "Shield Properties")
		UBoxComponent* ShieldBox;

	};
