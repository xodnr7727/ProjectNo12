// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterTypes.h"
#include "UISlot.generated.h"

class UImage;
class UTextBlock;
enum ESlotType;

/**
 * 
 */
UCLASS()
class PROJECTNO1_API UUISlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMax = 26, UIMin = -1))
		int SlotNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int Count;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TEnumAsByte<ESlotType> Type;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UImage* Img;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UTextBlock* Text;
};
