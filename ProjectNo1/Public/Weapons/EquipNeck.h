// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Characters/BaseCharacter.h"
#include "EquipNeck.generated.h"

class USoundBase;

UCLASS()
class PROJECTNO1_API AEquipNeck : public AItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEquipNeck();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* ItemIcon;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void EquipNeck(USceneComponent* InParent, FName InSocketName);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void PlayEquipSound();
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		USoundBase* EquipSound;

};
