// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProAnimInstance.h"
#include "ProjectNo1/ProjectNo1Character.h"
#include "ProjectNo1/LichEnemy.h"
#include "BossCharacter.h"
#include "CharacterTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UMyProAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	ProjectNo1Character = Cast<AProjectNo1Character>(TryGetPawnOwner());
	LichEnemy = Cast<ALichEnemy>(TryGetPawnOwner());
	BossCharacter = Cast<ABossCharacter>(TryGetPawnOwner());
	if (ProjectNo1Character)
	{
		ProjectNo1CharacterMovement = ProjectNo1Character->GetCharacterMovement();
	}
	if (LichEnemy) {
		LichEnemyMovement = LichEnemy->GetCharacterMovement();
	}
	if (BossCharacter) {
		BossCharacterMovement = BossCharacter->GetCharacterMovement();
	}
}
void UMyProAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (ProjectNo1Character == nullptr) {
		ProjectNo1Character = Cast<AProjectNo1Character>(TryGetPawnOwner());
	}
	if (LichEnemy == nullptr) {
		LichEnemy = Cast<ALichEnemy>(TryGetPawnOwner());
	}
	if (BossCharacter == nullptr) {
		BossCharacter = Cast<ABossCharacter>(TryGetPawnOwner());
	}
	if (ProjectNo1Character) {
		FVector Velocity{ ProjectNo1Character->GetVelocity() };
		Velocity.Z = 0;
		GroundSpeed = Velocity.Size();

		IsInAir = ProjectNo1Character->GetMovementComponent()->IsFalling();

		FRotator AimRotation = ProjectNo1Character->GetBaseAimRotation();

		FRotator MovementRotation =
			UKismetMathLibrary::MakeRotFromX(
				ProjectNo1Character->GetVelocity());

		MovementOffset = UKismetMathLibrary::NormalizedDeltaRotator(
			MovementRotation,
			AimRotation).Yaw;
	}
	if (ProjectNo1CharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(ProjectNo1CharacterMovement->Velocity);
		IsFalling = ProjectNo1CharacterMovement->IsFalling();
		CharacterState = ProjectNo1Character->GetCharacterState();
		ActionState = ProjectNo1Character->GetActionState();
	}
	if (LichEnemyMovement) {
		EnemyState = LichEnemy->GetEnemyState();
	}
	if (BossCharacterMovement) {
		EnemyState = BossCharacter->GetEnemyState();
	}

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn)) {
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
	}

	auto Character = Cast<ACharacter>(Pawn);
	if (Character)
	{
		IsInAir = Character->GetMovementComponent()->IsFalling();
		CurrentWalkSpeed = Character->GetMovementComponent()->GetMaxSpeed();
	}

}

void UMyProAnimInstance::AnimNotify_ArrowProjectile()
{
	ALichEnemy* pOwner = Cast<ALichEnemy>(TryGetPawnOwner());

	if (IsValid(pOwner))
		pOwner->ProjectileAttack();
}