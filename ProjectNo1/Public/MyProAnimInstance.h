// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "MyProAnimInstance.generated.h"
UCLASS()
class PROJECTNO1_API UMyProAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void AnimNotify_ArrowProjectile();

	UPROPERTY(BlueprintReadOnly)
	class AProjectNo1Character* ProjectNo1Character;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* ProjectNo1CharacterMovement;

	UPROPERTY(BlueprintReadOnly)
	class ALichEnemy* LichEnemy;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* LichEnemyMovement;

	UPROPERTY(BlueprintReadOnly)
	class ABossCharacter* BossCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* BossCharacterMovement;

	UPROPERTY(BlueprintReadOnly)
	class AGoblin* Goblin;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* GoblinMovement;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	bool IsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		bool IsInAir;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	EActionState ActionState;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	EEnemyState EnemyState;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	TEnumAsByte<EDeathPose> DeathPose;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		float CurrentWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, Meta = (AllowPrivateAccess = true))
		float MovementOffset;

};
