// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "GameFramework/Actor.h"
#include "Potion.generated.h"
class USoundBase;
class UBoxComponent;
UCLASS()
class PROJECTNO1_API APotion : public AItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APotion();

	UFUNCTION(BlueprintCallable)
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);

	void DeactivateEmbers();
	void DisableCapsuleCollision();
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	TArray<AActor*> IgnoreActors;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Potion Properties")
	USoundBase* PotionSound;

	UPROPERTY(EditAnywhere, Category = "Potion Properties")
	UParticleSystem* PotionParticles;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE UBoxComponent* GetPotionBox() const { return PotionBox; }

	UPROPERTY(EditAnywhere, Category = "Potion Properties")
	UBoxComponent* PotionBox;
};
