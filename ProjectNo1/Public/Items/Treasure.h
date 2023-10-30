// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Treasure.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNO1_API ATreasure : public AItem
{
	GENERATED_BODY()

protected:
	virtual void OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
		 
private:

	UPROPERTY(EditAnywhere, Category = "Treasure Properties")
		int32 Gold;
public:
	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE void SetGold(int32 AmountOfGold) { Gold = AmountOfGold; }
};
