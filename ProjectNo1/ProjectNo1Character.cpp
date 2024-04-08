// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectNo1Character.h"
#include "MyProAnimInstance.h"
#include "LichEnemy.h"
#include "BossCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/World.h"
#include "Item.h"
#include "Items/Treasure.h"
#include "Weapons/MySkillClass.h"
#include "Weapons/Weapon.h"
#include "Weapons/Shield.h"
#include "Weapons/Potion.h"
#include "Weapons/ProjectileWeapon.h"
#include "HUD/MyProNo1HUD.h"
#include "HUD/SlashOverlay.h"
#include "Soul.h"

//////////////////////////////////////////////////////////////////////////
// AProjectNo1Character

AProjectNo1Character::AProjectNo1Character()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	RageSkillEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RageSkillEffect"));
	RageSkillEffect->SetupAttachment(RootComponent); // 이펙트 위치 설정

	PotionSkillEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PotionSkillEffect"));
	PotionSkillEffect->SetupAttachment(RootComponent); // 이펙트 위치 설정

	CurrentComboStep = 0; //콤보 공격
	ComboSectionNames.Add(FName("Combo01"));
	ComboSectionNames.Add(FName("Combo02"));
	AttackComboCount = 3.0f;//콤보 초기화 시간
	//ComboSectionNames.Add(FName("Combo03"));

	PotionCooldown = 10.0f; // 초기 쿨타임 설정 (예: 10초)
	bCanDrinkPotion = true; // 초기에 포션 마실 수 있도록 설정
	PotionDuration = 5.0f; //포션 지속시간 설정 (예: 5초)

    DiveCooldown = 1.0f; // 초기 쿨타임 설정 (예: 1초)
	bCanDive = true; // 초기에 구르기 할 수 있도록 설정

	RageCooldown = 15.0f; // 초기 쿨타임 설정 (예: 15초)
	bCanRage = true; // 초기에 분노 사용할 수 있도록 설정
	RageDuration = 8.0f;// 분노 지속 시간

	SwordSkillCooldown = 8.0f; // 초기 쿨타임 설정 (예: 8초)
	SwordSkillDuration = 6.0f; // 초기 지속시간 설정 (예: 6초)
	bCanSwordSkill = true; // 초기에 검 공격 사용할 수 있도록 설정

	LargeSkillCooldown = 8.0f; // 초기 쿨타임 설정 (예: 8초)
	bCanLargeSkill = true; // 초기에 강공격 사용할 수 있도록 설정

	SmallSkillCooldown = 6.0f; // 초기 쿨타임 설정 (예: 6초)
	bCanSmallSkill = true; // 초기에 약공격 사용할 수 있도록 설정

	ParryCountdown = 2.0f; //패리기능 쿨타임
	bCanParry = true; //초기 패리기능 사용가능

	// Set the default value for the special targeting range
	SpecialTargetingRange = 200.0f;

	// Initialize the special targeting flag
	bIsSpecialTargetingEnabled = false;

	GuardCounterDisableTimer = 2.0f; //가드 카운터 타이머
	bIsGuardCounterAttackEnabled = false; //가드 카운터 부울 변수
}


void AProjectNo1Character::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)//맞는 함수
{
	float AngleToMonster = CalculateAngleBetweenPlayerAndMonster(this, Hitter);
	if (ActionState == EActionState::EAS_Blocking && HasEnoughShieldStamina() && AngleToMonster <= MaxParryAngle)return;
	//막기 상태이고 스태미너가 충분하고 플레이어의 막는 위치가 앞일때 리턴
		Super::GetHit_Implementation(ImpactPoint, Hitter);

		SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
		ActionState = EActionState::EAS_HitReaction;
}

void AProjectNo1Character::GetBlock_Implementation(const FVector& ImpactPoint, AActor* Hitter)//막는 함수
{
	float AngleToMonster = CalculateAngleBetweenPlayerAndMonster(this, Hitter);
	if (!(ActionState == EActionState::EAS_Blocking && HasEnoughShieldStamina() && AngleToMonster <= MaxParryAngle)) return;
	if (Attributes && SlashOverlay)
	{
		Attributes->UseStamina(Attributes->GetShieldCost());
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
		Super::GetBlock_Implementation(ImpactPoint, Hitter);

		SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
		EnableGuardCounter(); //가드 카운터 활성화
		ActionState = EActionState::EAS_Blocking;
}

void AProjectNo1Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Attributes && SlashOverlay)
	{
		if (ActionState == EActionState::EAS_Unoccupied) {//기본 상태일때만 체력이나 스태미너 재생
			Attributes->RegenHealth(DeltaTime);
			Attributes->RegenStamina(DeltaTime);
			SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
			SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}
		if (ActionState == EActionState::EAS_Sprint) {//달리기중일때 스태미너 초당 소모
			Attributes->RegenMinusStamina(DeltaTime);
			SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		SlashOverlay->SetExperienceBarPercent(Attributes->GetExperiencePercent());
	}

	if (!HasEnoughStamina() && ActionState == EActionState::EAS_Sprint) { //달리기중일때 스태미너가 부족하면 달리기 해제
		GetCharacterMovement()->MaxWalkSpeed = 300.f;

		ActionState = EActionState::EAS_Unoccupied;
	}

	if (HasStunnedEnemyInFront())//플레이어 앞에 스턴된 적이 있다면
	{
		EnableSpecialTargetingAttack();//스턴 공격 허용
	}
	else if (!HasStunnedEnemyInFront()) //플레이어 앞에 스턴된 적이 없다면
	{
		DisableSpecialTargetingAttack();//스턴 공격 불가
	}
}

void AProjectNo1Character::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void AProjectNo1Character::AddEx(ASoul* Soul) //경험치 추가
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SlashOverlay->SetExperienceBarPercent(Attributes->GetExperiencePercent());
		SlashOverlay->SetLevel(Attributes->GetLevel());
	}
}

void AProjectNo1Character::AddGold(ATreasure* Treasure) //골드 추가
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		SlashOverlay->SetGold(Attributes->GetGold());
	}
}

void AProjectNo1Character::BeginPlay()
{
	Super::BeginPlay();
	RageSkillEffect->DeactivateSystem(); // 초기에는 비활성화
	PotionSkillEffect->DeactivateSystem(); // 초기에는 비활성화
	Tags.Add(FName("EngageableTarget"));
	InitializeSlashOverlay();//기본 UI 설정
	SpawnDefaultWeapon();//주무기 장착
	SpawnDefaultWeaponTwo();//쉴드 장착
	SpawnDefaultPotionOne();//포션 장착

	if (HasStunnedEnemyInFront())//플레이어 앞에 스턴된 적이 있다면
	{
		EnableSpecialTargetingAttack();//스턴 공격 허용
	}
	else if (!HasStunnedEnemyInFront())//플레이어 앞에 스턴된 적이 없다면
	{
		DisableSpecialTargetingAttack();//스턴 공격 불가
	}
}

void AProjectNo1Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AProjectNo1Character::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AProjectNo1Character::LMBDown);
	//PlayerInputComponent->BindAction("LMB", IE_Released, this, &AProjectNo1Character::LMBUp);

	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &AProjectNo1Character::Attack);
	//PlayerInputComponent->BindAction(FName("Attack"), IE_Released, this, &AProjectNo1Character::LMBUp);

	PlayerInputComponent->BindAction(FName("Block"), IE_Pressed, this, &AProjectNo1Character::RMKeyPressed);
	PlayerInputComponent->BindAction(FName("Block"), IE_Released, this, &AProjectNo1Character::RMKeyReleased);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AProjectNo1Character::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AProjectNo1Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AProjectNo1Character::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AProjectNo1Character::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AProjectNo1Character::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AProjectNo1Character::TouchStopped);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AProjectNo1Character::EKeyPressed);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AProjectNo1Character::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AProjectNo1Character::StopSprinting);

	PlayerInputComponent->BindAction(FName("Dive"), IE_Pressed, this, &AProjectNo1Character::Dive);

	PlayerInputComponent->BindAction(FName("NeckSkill"), IE_Pressed, this, &AProjectNo1Character::OnNeckSkillPressed);

	PlayerInputComponent->BindAction(FName("SwordSkill"), IE_Pressed, this, &AProjectNo1Character::OnSwordSkillPressed);

	PlayerInputComponent->BindAction(FName("DrinkPotion"), IE_Pressed, this, &AProjectNo1Character::DrinkPotion);

	PlayerInputComponent->BindAction(FName("LargeSkill"), IE_Pressed, this, &AProjectNo1Character::LargeSkillPressed);

	PlayerInputComponent->BindAction(FName("Parry"), IE_Pressed, this, &AProjectNo1Character::Parry); 

	PlayerInputComponent->BindAction(FName("SpecialTargetingAttack"), IE_Pressed, this, &AProjectNo1Character::SpecialTargetingAttackInput);

	PlayerInputComponent->BindAction(FName("GuardCounter"), IE_Pressed, this, &AProjectNo1Character::GuardCounterPressed);
}

void AProjectNo1Character::OnNeckSkillPressed()
{
	FTimerHandle NeckSkillCountdown;
	FTimerHandle RageEndTimerHandle;
	FTimerHandle RageEffectEndTimerHandle;

	if (CanNeckSkill() && HasEnoughSkillStamina() && bCanRage)
	{
		PlayNeckSkillMontage();
		ActionState = EActionState::EAS_NeckSkillDo;
		bCanRage = false;//연속 사용 불가
		EquippedWeapon->IncreaseDamage();// 공격력 증가
		RageSkillEffect->ActivateSystem();//분노 이펙트 활성화
		GetWorldTimerManager().SetTimer(NeckSkillCountdown, this, &AProjectNo1Character::EnableRage, RageCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(RageEndTimerHandle, this, &AProjectNo1Character::RestoreDamage, RageDuration, false);//분노 지속시간 타이머 시작
		GetWorldTimerManager().SetTimer(RageEffectEndTimerHandle, this, &AProjectNo1Character::DeactivateSkillEffect, RageDuration, false);//이펙트 지속시간 타이머 시작
		if (Attributes && SlashOverlay)
		{
			Attributes->UseStamina(Attributes->GetSkillCost());
			SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}
	}
}

void AProjectNo1Character::DeactivateSkillEffect()
{
	RageSkillEffect->DeactivateSystem();// 분노 이펙트 비활성화
}

void AProjectNo1Character::RestoreDamage()
{
	EquippedWeapon->RestoreDamage();// 공격력 복구
}

void AProjectNo1Character::EnableRage()
{
	bCanRage = true; // 쿨타임이 끝나면 다시 스킬을 사용할 수 있도록 설정
}

void AProjectNo1Character::LargeSkillPressed()
{
	FTimerHandle LargeSkillCountdown;

	if (CanNeckSkill() && HasEnoughSkillStamina() && bCanLargeSkill)
	{
		PlayLargeSkillMontage();
		ActionState = EActionState::EAS_LargeAttack;
		bCanLargeSkill = false;//연속 사용 X
		GetWorldTimerManager().SetTimer(LargeSkillCountdown, this, &AProjectNo1Character::EnableLargeSkill, LargeSkillCooldown, false); // 쿨타임 타이머 시작
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//강공격 시 무적
		
		if (EquippedWeapon)
		{
			EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
		}

		if (Attributes && SlashOverlay)
		{
			Attributes->UseStamina(Attributes->GetSkillCost());
			SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}
	}
}

void AProjectNo1Character::EndLargeAttack()
{
	
	ActionState = EActionState::EAS_Unoccupied;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제

	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandWeaponSocket"));
	}
}
void AProjectNo1Character::EnableLargeSkill()
{
	bCanLargeSkill = true;// 강공격 사용 활성화
}

void AProjectNo1Character::SwingSword()
{
	if (EquippedProjectile) {
		EquippedProjectile->SetActorHiddenInGame(true);
	}
	if (SwordProjectileClass)
	{
		UE_LOG(LogTemp, Log, TEXT("SwingSword"));

		const FRotator SpawnRotation = GetActorRotation();
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(FVector(80.0f, 30.0f, 10.0f));
		UWorld* const World = GetWorld();
		if (World != NULL) {

			AProjectileWeapon* SpawnedProjectile = World ->SpawnActor<AProjectileWeapon>(SwordProjectileClass, SpawnLocation, SpawnRotation);
			UE_LOG(LogTemp, Log, TEXT("SpawnActor"));
			if (SpawnedProjectile) {
				SpawnedProjectile->GetProjectileMovement()->SetVelocityInLocalSpace(FVector::ForwardVector * 2000.0f);
			}
		}
	}
}

void AProjectNo1Character::GuardCounterPressed()
{
	if (CanNeckSkill() && HasEnoughSkillStamina() && bIsGuardCounterAttackEnabled)
	{
		PlayGuardCounterMontage();
		ActionState = EActionState::EAS_AttackSkill;
		bIsGuardCounterAttackEnabled = false;//연속 사용 X
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//카운터 공격 시 무적
		EquippedWeapon->IncreaseCounterDamage();// 공격력 증가
		UE_LOG(LogTemp, Log, TEXT("GuardCounter"));
		if (Attributes && SlashOverlay)
		{
			Attributes->UseStamina(Attributes->GetSkillCost());
			SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}
	}
}

void AProjectNo1Character::DeactivateGuardCounterEffect()
{
	EquippedWeapon->DeactivateGuardCounterEffect(); // 카운터 공격 이펙트 비활성화
	EquippedWeapon->RestoreCounterDamage(); // 공격력 복구
}

void AProjectNo1Character::ActivateGuardCounterEffect()
{
	EquippedWeapon->ActivateGuardCounterEffect(); // 카운터 공격 이펙트 활성화
}

void AProjectNo1Character::EnableGuardCounter()
{
	FTimerHandle GuardCounterCountdown;

	bIsGuardCounterAttackEnabled = true;// 가드 카운터 활성화
	GetWorldTimerManager().SetTimer(GuardCounterCountdown, this, &AProjectNo1Character::DisableGuardCounter, GuardCounterDisableTimer, false);
	//일정 시간 내에 가드 카운터 공격하지 않을 시 비활성화
}

void AProjectNo1Character::DisableGuardCounter()
{
	bIsGuardCounterAttackEnabled = false;// 가드 카운터 비활성화
}

void AProjectNo1Character::SmallSkillPressed()
{
	FTimerHandle SmallSkillCountdown;

	if (CanNeckSkill() && HasEnoughSkillStamina() && bCanSmallSkill)
	{
		PlaySmallSkillMontage();
		ActionState = EActionState::EAS_AttackSkill;
		bCanSmallSkill = false;//연속 사용 X
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//스턴 공격 시 무적
		EquippedWeapon->IncreaseStunDamage();// 공격력 증가
		UE_LOG(LogTemp, Log, TEXT("StunAttack"));
		GetWorldTimerManager().SetTimer(SmallSkillCountdown, this, &AProjectNo1Character::EnableSmallSkill, SmallSkillCooldown, false); // 쿨타임 타이머 시작
		if (Attributes && SlashOverlay)
		{
			Attributes->UseStamina(Attributes->GetSkillCost());
			SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}
	}
}

void AProjectNo1Character::EndSmallSkill()
{
	ActionState = EActionState::EAS_Unoccupied;
	EquippedWeapon->RestoreStunDamage(); // 공격력 복구
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제
}

void AProjectNo1Character::EnableSmallSkill()
{
	bCanSmallSkill = true;// 약공격 활성화
}

void AProjectNo1Character::ActivateSmallSkillEffect()
{
	EquippedWeapon->ActivateSmallSkillEffect(); // 약공격 이펙트 활성화
}

void AProjectNo1Character::DeactivateSmallSkillEffect()
{
	EquippedWeapon->DeactivateSmallSkillEffect(); // 약공격 이펙트 비활성화
}

bool AProjectNo1Character::IsAttackSkill()
{
	return ActionState == EActionState::EAS_AttackSkill ||
		ActionState == EActionState::EAS_Attacking ||
		ActionState == EActionState::EAS_Parrying;
}

void AProjectNo1Character::OnSwordSkillPressed()
{
	FTimerHandle SwordSkillCountdown;
	FTimerHandle SwordSkillAGITime;

	if (CanNeckSkill() && HasEnoughSkillStamina() && bCanSwordSkill)
	{
		PlaySwordSkillMontage();
		ActionState = EActionState::EAS_AttackSkill;
		bCanSwordSkill = false;//연속 사용 X
		EquippedWeapon->IncreaseSkillDamage();// 공격력 증가
		GetWorldTimerManager().SetTimer(SwordSkillCountdown, this, &AProjectNo1Character::EnableSwordSkill, SwordSkillCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(SwordSkillAGITime, this, &AProjectNo1Character::DeactivateLargeSkillEffect, SwordSkillDuration, false); // 지속시간 타이머 시작
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//검 공격 시 무적
		PlayWeaponSkillSound(); //스킬 사운드 재생
		ActivateSkillParticles(); // 스킬 이펙트 재생
		if (Attributes && SlashOverlay)
		{
			Attributes->UseStamina(Attributes->GetSkillCost());
			SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}
	}
}

void AProjectNo1Character::EndSwordSkill()
{
	ActionState = EActionState::EAS_Unoccupied;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제
}

void AProjectNo1Character::EnableSwordSkill()
{
	bCanSwordSkill = true;// 강공격 사용 활성화
}

void AProjectNo1Character::DeactivateLargeSkillEffect()
{
	EquippedWeapon->DeactivateLargeSkillEffect(); // 강공격 이펙트 비활성화
	EquippedWeapon->DeactivateSmallSkillEffect(); // 약공격 이펙트 비활성화
	EquippedWeapon->RestoreSkillDamage(); // 공격력 복구
}

void AProjectNo1Character::ActivateLargeSkillEffect()
{
	EquippedWeapon->ActivateLargeSkillEffect(); // 강공격 이펙트 활성화
	EquippedWeapon->ActivateSmallSkillEffect(); // 약공격 이펙트 활성화
}

void AProjectNo1Character::ActivateSkillParticles()
{
	if (SkillParticles && GetWorld())
	{
		FVector SpawnLocation = GetActorLocation() - FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		//플레이어 발바닥에 생성되도록

		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			SkillParticles,
			SpawnLocation
		);
	}
}

void AProjectNo1Character::ActivateLevelParticles()
{
	if (SkillParticles && GetWorld())
	{
		FVector SpawnLocation = GetActorLocation() - FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		//플레이어 발바닥에 생성되도록

		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			SkillParticles,
			SpawnLocation
		);
	}
}

void AProjectNo1Character::EquipNeck(AWeapon* NewNeck)
{
	EquippedNeck = NewNeck;
}

void AProjectNo1Character::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}

}

void AProjectNo1Character::Sprint()//달리기
{
	if (HasEnoughStamina() && ActionState == EActionState::EAS_Unoccupied) {
		GetCharacterMovement()->MaxWalkSpeed = 450.f;

		ActionState = EActionState::EAS_Sprint;
	}
}

void AProjectNo1Character::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.f;

	ActionState = EActionState::EAS_Unoccupied;
} 

float AProjectNo1Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (ActionState == EActionState::EAS_Blocking && HasEnoughShieldStamina()) { //막기 상태일때 데미지 0
		HandleDamage(0);
		SetHUDHealth();
		return 0;
	}
	else { //막기 상태 아닐때 데미지 받기
		HandleDamage(DamageAmount);
		SetHUDHealth();
		return DamageAmount;
	}
}

void AProjectNo1Character::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AProjectNo1Character::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AProjectNo1Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AProjectNo1Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AProjectNo1Character::MoveForward(float Value)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsAttackSkill()) return; //공격중일때 이동 불가
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AProjectNo1Character::MoveRight(float Value)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsAttackSkill()) return; //공격중일때 이동 불가
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AProjectNo1Character::RMKeyPressed()
{
	if (!(HasEnoughShieldStamina()) || ActionState == EActionState::EAS_Sprint)return; //쉴드에 충분한 스태미너가 아니거나 달리기 상태 일때 리턴
	if (CanBlock())
	{
		DisBlock();
	}
}

void AProjectNo1Character::RMKeyReleased()
{
	if (ActionState == EActionState::EAS_Blocking)
	{
		AsBlock();
	}
}

void AProjectNo1Character::IfAttack()
{
	if (ActionState == EActionState::EAS_Unoccupied)
	{
		Attack();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AProjectNo1Character::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	AProjectileWeapon* OverlappingProjectileWeapon = Cast<AProjectileWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
		EquipProjectileWeapon(OverlappingProjectileWeapon);
	}
	else
	{
		if (CanDisarm())
		{
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
	}
}

void AProjectNo1Character::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("SpineSocket"), this, this);
	CharacterState = ECharacterState::ECS_Unequipped;
	//Weapon->Equip(GetMesh(), FName("RightHandWeaponSocket"), this, this);
	//CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

void AProjectNo1Character::EquipProjectileWeapon(AProjectileWeapon* ProjectileWeapon)
{
	ProjectileWeapon->ProjectileEquip(GetMesh(), this, this);
	CharacterState = ECharacterState::ECS_Unequipped;
	//Weapon->Equip(GetMesh(), FName("RightHandWeaponSocket"), this, this);
	//CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedProjectile = ProjectileWeapon;
}

void AProjectNo1Character::AttachWeaponToBack()//무기 등으로
{
	if (EquippedWeapon) 
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void AProjectNo1Character::AttachWeaponToHand()//무기 손으로
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandWeaponSocket"));
	}
}

bool AProjectNo1Character::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDiveCost();
}

bool AProjectNo1Character::HasEnoughPotionStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetPotionCost();
}

bool AProjectNo1Character::HasEnoughAttackStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetAttackCost();
}

bool AProjectNo1Character::HasEnoughSkillStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetSkillCost();
}

bool AProjectNo1Character::HasEnoughShieldStamina()//쉴드를 올릴 스태미너가 충분한가
{
	return Attributes && Attributes->GetStamina() > Attributes->GetShieldCost();
}

bool AProjectNo1Character::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

bool AProjectNo1Character::CanDisarm()//해제할수있는 상태인가
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool AProjectNo1Character::CanArm()//장착할수있는 상태인가
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped && EquippedWeapon;
}

bool AProjectNo1Character::CanBlock()
{
	//return ActionState == EActionState::EAS_Unoccupied &&
		return CharacterState != ECharacterState::ECS_Unequipped;
}

void AProjectNo1Character::Disarm()//무기 해제
{
	PlayEquip(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AProjectNo1Character::Arm() //무기 장착
{
	PlayEquip(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AProjectNo1Character::DisBlock()
{
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_Blocking;
}

void AProjectNo1Character::AsBlock()
{
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_Unoccupied;
}

void AProjectNo1Character::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AProjectNo1Character::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool AProjectNo1Character::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void AProjectNo1Character::InitializeSlashOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		AMyProNo1HUD* MyProNo1HUD = Cast<AMyProNo1HUD>(PlayerController->GetHUD());
		if (MyProNo1HUD)
		{
			SlashOverlay = MyProNo1HUD->GetSlashOverlay();
			if (SlashOverlay && Attributes)
			{
				SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				SlashOverlay->SetStaminaBarPercent(1.f);
				SlashOverlay->SetExperienceBarPercent(Attributes->GetExperiencePercent());
			}
		}
	}
}

void AProjectNo1Character::SetHUDHealth()
{
	if (SlashOverlay && Attributes)
	{
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void AProjectNo1Character::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AProjectNo1Character::BlockEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AProjectNo1Character::Parry()
{
	FTimerHandle ParryCount;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (CanAttack() && HasEnoughAttackStamina())
	{
		AnimInstance->Montage_Play(ParryMontage);
		bCanParry= false; // 패리 한 후 연속 패리 x
		GetWorldTimerManager().SetTimer(ParryCount, this, &AProjectNo1Character::ParryCanDo, ParryCountdown, false); //패리 쿨타임 타이머 시작
		ActionState = EActionState::EAS_Parrying;
		UE_LOG(LogTemp, Log, TEXT("Parry"));
		if (Attributes && SlashOverlay) //공격 스태미너 소모
		{
			Attributes->UseStamina(Attributes->GetAttackCost());
			SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}
	}
}
void AProjectNo1Character::ParryCanDo()
{
	bCanParry = true; //패리 가능
}

bool AProjectNo1Character::HasStunnedEnemyInFront()//플레이어 앞 적 스턴 체크 함수
{
	TArray<FHitResult> HitResults;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * SpecialTargetingRange;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // Ignore the player
	CollisionParams.bTraceComplex = true;
	bool bHit = GetWorld()->SweepMultiByChannel(HitResults, StartLocation, EndLocation, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(50.0f), CollisionParams);

	if (bHit)
	{
		for (const FHitResult& HitResult : HitResults)
		{
			AActor* OverlappingActor = HitResult.GetActor();
			// 액터의 클래스가 ALichEnemy인지 확인
			if (OverlappingActor->IsA(ALichEnemy::StaticClass()))
			{
				LichEnemy = Cast<ALichEnemy>(OverlappingActor);

				if (LichEnemy && LichEnemy->IsStunned())
				{
					//UE_LOG(LogTemp, Log, TEXT("LichEnemyStunnedCheck")); //확인완료
					return true;
				}
				else if(LichEnemy && LichEnemy->IsNotStunned()){
					//UE_LOG(LogTemp, Log, TEXT("LichEnemyNotStunnedCheck"));//확인완료
					return false;
				}
			}
		}
	}
	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 2.0f); //확인완료
	return false;
}

void AProjectNo1Character::EnableSpecialTargetingAttack()
{
	bIsSpecialTargetingEnabled = true; //스턴 공격 가능
}

void AProjectNo1Character::DisableSpecialTargetingAttack()
{
	bIsSpecialTargetingEnabled = false; //스턴 공격 불가
}

void AProjectNo1Character::SpecialTargetingAttackInput()
{
	if (bIsSpecialTargetingEnabled)
	{

		SmallSkillPressed();
		bIsSpecialTargetingEnabled = false; //연속 스턴 공격 x
	}
}

void AProjectNo1Character::Attack()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	Super::Attack();
	
	if (CanAttack() && HasEnoughAttackStamina())
	{
		FName SectionName = ComboSectionNames[CurrentComboStep];
		AnimInstance->Montage_Play(AttackMontage);
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
		CurrentComboStep = (CurrentComboStep + 1) % ComboSectionNames.Num(); // 다음 콤보 스텝으로 이동 (콤보가 끝난 경우 0으로 초기화)
		ActionState = EActionState::EAS_Attacking;
		UE_LOG(LogTemp, Log, TEXT("Attack"));
		if (Attributes && SlashOverlay) //공격 스태미너 소모
		{
			Attributes->UseStamina(Attributes->GetAttackCost());
			SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}
	}
}
void AProjectNo1Character::AttackComboResetCountDown()
{
	GetWorldTimerManager().ClearTimer(AttackComboCountdown); //공격 키 재입력시 콤보 초기화 타이머 초기화
}

void AProjectNo1Character::AttackComboStartCountDown()
{
	GetWorldTimerManager().SetTimer(AttackComboCountdown, this, &AProjectNo1Character::AttackComboReset, AttackComboCount, false); // 콤보 초기화 타이머 시작
}
void AProjectNo1Character::AttackComboReset()
{
	CurrentComboStep = 0; //콤보 공격 리셋
}

void AProjectNo1Character::Dive()
{
	FTimerHandle DiveCountdown;

	if (CanAttack() && HasEnoughStamina() && bCanDive) {
		PlayDiveMontage();
		ActionState = EActionState::EAS_Dive;
		UE_LOG(LogTemp, Log, TEXT("Dive"));
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//구르기 시 무적
		GetCharacterMovement()->MaxWalkSpeed = 500.f;
		bCanDive = false; // 구르기 한 후 연속 구르기 x
		GetWorldTimerManager().SetTimer(DiveCountdown, this, &AProjectNo1Character::EnableDive, DiveCooldown, false); // 쿨타임 타이머 시작

		if (Attributes && SlashOverlay)//구르기 스태미너 소모
		{
			Attributes->UseStamina(Attributes->GetDiveCost());
			SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}
	}
}

bool AProjectNo1Character::IsDiving()
{
	return ActionState == EActionState::EAS_Dive;
}

void AProjectNo1Character::EnableDive()
{
	bCanDive = true; // 쿨타임이 끝나면 다시 구르기를 할 수 있도록 설정
}

void AProjectNo1Character::DiveEnd()
{
	Super::DiveEnd();

	ActionState = EActionState::EAS_Unoccupied;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

void AProjectNo1Character::DrinkPotion()//포션 마시기
{
	FTimerHandle PotionCountdown;
	FTimerHandle PotionEffectEndTimerHandle;

	if (Unequipoccupied() && HasEnoughPotionStamina() && bCanDrinkPotion)
	{
		// 포션을 마시는 로직 추가
		PlayDrinkMontage();
		ActionState = EActionState::EAS_Drink;
		UE_LOG(LogTemp, Log, TEXT("DrinkPotion"));
		bCanDrinkPotion = false; // 포션을 마신 후, 연속으로 마실 수 없도록 설정
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//포션 마실 시 무적
		GetWorldTimerManager().SetTimer(PotionCountdown,this,&AProjectNo1Character::EnablePotion, PotionCooldown,false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(PotionEffectEndTimerHandle, this, &AProjectNo1Character::DeactivatePotionEffect, PotionDuration, false);//포션 지속시간 타이머 시작
		EquippedPotion->ActivateEffect();
		if (EquippedPotion)
		{
			EquippedPotion->AttachMeshToSocket(GetMesh(), FName("RightHandPotionSocket"));
		}
		if (EquippedWeapon)
		{
			EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
		}
		if (Attributes && SlashOverlay)
		{
			Attributes->UseStamina(Attributes->GetPotionCost());
			SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}
	}
}
void AProjectNo1Character::DrinkEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제
	EquippedPotion->DeactivateEffect();
	if (EquippedPotion)
	{
		EquippedPotion->AttachMeshToSocket(GetMesh(), FName("RightBeltPotionSocket"));
	}
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandWeaponSocket"));
	}
}

void AProjectNo1Character::DeactivatePotionEffect()
{
	if (PotionSkillEffect) {
		PotionSkillEffect->DeactivateSystem(); //포션 이펙트 비활성화
	}
	Attributes->BaseHealth();//체력 재생률 원상복귀
}

void AProjectNo1Character::EnablePotion()
{
	bCanDrinkPotion = true; // 쿨타임이 끝나면 다시 포션 마실 수 있도록 설정
}

void AProjectNo1Character::DrinkHealthPotion() {

	if (PotionSkillEffect) {
		PotionSkillEffect->ActivateSystem();//포션 이펙트 활성화
	}
	Attributes->RecoveryHealth();//체력 재생률 증가
}

bool AProjectNo1Character::Unequipoccupied()//무장해제 상태
{
	return ActionState == EActionState::EAS_Unoccupied;
	
}

bool AProjectNo1Character::IsArm()
{
	return CharacterState == ECharacterState::ECS_EquippedOneHandedWeapon;
}

bool AProjectNo1Character::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool AProjectNo1Character::CanNeckSkill()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

void AProjectNo1Character::PlayEquip(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipUnEquipMontage)
	{
		AnimInstance->Montage_Play(EquipUnEquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipUnEquipMontage);
	}
}

void AProjectNo1Character::EndAttacking()
{
	ActionState = EActionState::EAS_Unoccupied;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제
}

void AProjectNo1Character::EndBlocking()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AProjectNo1Character::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("SpineSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void AProjectNo1Character::SpawnDefaultWeaponTwo()
{
	UWorld* World = GetWorld();
	if (World && ShieldClass)
	{
		AShield* DefaultShield = World->SpawnActor<AShield>(ShieldClass);
		DefaultShield->Equip(GetMesh(), FName("LeftHandWeaponSocket"), this, this);
		EquippedShield = DefaultShield;
	}
}

void AProjectNo1Character::SpawnDefaultPotionOne()
{
	UWorld* World = GetWorld();
	if (World && PotionClass)
	{
		APotion* DefaultPotion = World->SpawnActor<APotion>(PotionClass);
		DefaultPotion->Equip(GetMesh(), FName("RightBeltPotionSocket"), this, this);
		EquippedPotion = DefaultPotion;
	}
}

