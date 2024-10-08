// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectNo1Character.h"
#include "MyProAnimInstance.h"
#include "ProjectNo1GameMode.h"
#include "LichEnemy.h"
#include "BossCharacter.h"
#include "Goblin.h"
#include "CaveEnemy.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AttributeComponent.h"
#include "MyProGameInstance.h"
#include "MyProSaveGame.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
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
#include "BlessingPoint.h"
#include "Items/Treasure.h"
#include "Weapons/MySkillClass.h"
#include "Weapons/Weapon.h"
#include "Weapons/Shield.h"
#include "Weapons/Potion.h"
#include "Weapons/ProjectileWeapon.h"
#include "HUD/MyProNo1HUD.h"
#include "HUD/InventoryUI.h"
#include "HUD/GameRetryUI.h"
#include "HUD/SlashOverlay.h"
#include "HUD/DamageIncreaseWidget.h"
#include "HUD/AllMenuWidget.h"
#include "HUD/MapWidget.h"
#include "HUD/InfoWidget.h"
#include "HUD/SystemWidget.h"
#include "MyPlayerController.h"
#include "Soul.h" 
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "NiagaraFunctionLibrary.h"

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
	RageDuration = 11.0f;// 분노 지속 시간

	SwordSkillCooldown = 10.0f; // 초기 쿨타임 설정 (예: 10초)
	SwordSkillDuration = 8.0f; // 초기 지속시간 설정 (예: 8초)
	bCanSwordSkill = true; // 초기에 검 공격 사용할 수 있도록 설정

	WeaponSpellCooldown = 20.0f; // 초기 쿨타임 설정 (예: 20초)
	WeaponSpellCountdown = 14.0f; // 초기 지속시간 설정 (예: 14초)
	bCanWeaponSpell = true; // 초기에 주문 공격 사용할 수 있도록 설정
	AttackWeaponSpell = false; // 초기에 주문 공격 투사체가 나가지 않도록 설정

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

	GuardCounterCooldown = 6.0f; // 초기 쿨타임 설정 (예: 6초)
	GuardCounterDisableTimer = 2.0f; //가드 카운터 제한 시간 타이머
	bIsGuardCounterAttackEnabled = false; //가드 카운터 부울 변수
	bIsGuardCountdownEnabled = true;

	LevelUpCountdown = 0.5f; // 초기 쿨타임 설정 (예: 6초)

	EnableHitCountdown = 2.0f;// 무적 시간 타이머

	DamagebackCountdown = 2.0f;// 데미지 복구 타이머

	PushBackDistance = 200.0f; // 원하는 거리로 설정

	bIsAllMenuUIVisible = false;

	bPlayerDead = false;

	bDamageIncreaseState = true;

	bIsBlessingPointInteractEnabled = false;
}

void AProjectNo1Character::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)//맞는 함수
{
	float AngleToMonster = CalculateAngleBetweenPlayerAndMonster(this, Hitter);
	if (ActionState == EActionState::EAS_Blocking && HasEnoughShieldStamina() && AngleToMonster <= MaxParryAngle)return;
	//막기 상태이고 스태미너가 충분하고 플레이어의 막는 위치가 앞일때 리턴
		Super::GetHit_Implementation(ImpactPoint, Hitter);

		SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
		if (Attributes && Attributes->GetHealthPercent() > 0.f)
		{
			ActionState = EActionState::EAS_HitReaction;
		}
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

		BlockBack();
		SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
		EnableGuardCounter(); //가드 카운터 활성화
		ActionState = EActionState::EAS_Blocking;
}

float AProjectNo1Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float AngleToMonster = CalculateAngleBetweenPlayerAndMonster(this, DamageCauser);
	if (ActionState == EActionState::EAS_Blocking && HasEnoughShieldStamina() && AngleToMonster <= MaxParryAngle) { //막기 상태일때 데미지 0
		HandleDamage(0);
		SetHUDHealth();
		return 0;
	}
	else { //막기 상태 아닐때 데미지 받기
		HandleDamage(DamageAmount - Amor);
		SetHUDHealth();
		return DamageAmount - Amor;
	}
}

void AProjectNo1Character::BlockBack() //막기 넉백 함수
{
	FVector LaunchVelocity = GetActorForwardVector() * -500.0f;
	LaunchCharacter(LaunchVelocity, true, true);
}

void AProjectNo1Character::ExecuteGetHit(FHitResult& HitResult)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(HitResult.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetHit(HitResult.GetActor(), HitResult.ImpactPoint, GetOwner());
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

void AProjectNo1Character::LevelUpAll()
{
	FTimerHandle LevelCountdown;
	LevelUpEC();
	GetWorldTimerManager().SetTimer(LevelCountdown, this, &AProjectNo1Character::LevelUpES, LevelUpCountdown, false); // 카운드다운 타이머 시작
}

void AProjectNo1Character::LevelUpEC()
{
	ActionState = EActionState::EAS_LevelUp;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//무적
	GetCharacterMovement()->Deactivate();
}

void AProjectNo1Character::LevelUpES()
{
	ActionState = EActionState::EAS_Unoccupied;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제
	GetCharacterMovement()->Activate();
}

void AProjectNo1Character::AddGold(ATreasure* Treasure) //골드 추가
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		SlashOverlay->SetGold(Attributes->GetGold());
	}
}
void AProjectNo1Character::ICDamageGold() //데미지 증가
{
	if(bDamageIncreaseState){
		if (Attributes && SlashOverlay)
		{
			if (Attributes->GetGold() > 1999.f) {
				Attributes->ICDamageMinusGold();
				SlashOverlay->SetGold(Attributes->GetGold());
				WeaponDamage += 20.0f;
				DamageIncreaseWidgetInstance->SetDamage(GetWeaponDamage());
				InfoWidgetInstance->SetInfoDamage(GetWeaponDamage());
				PlayDamageIncreaseSound();
			}
			else {
				if (DamageIncreaseWidgetInstance)
				{
					DamageIncreaseWidgetInstance->ShowNotEnoughGoldMessage();
					PlayDamageIncreaseFailSound();
				}
			}
		}
    }
}

void AProjectNo1Character::ICAmorGold() //아머 증가
{
	if (Attributes && SlashOverlay)
	{
		if (Attributes->GetGold() > 1999.f) {
			Attributes->ICDamageMinusGold();
			SlashOverlay->SetGold(Attributes->GetGold());
			Amor += 0.5f;
		    DamageIncreaseWidgetInstance->SetAmor(GetAmor());
			InfoWidgetInstance->SetInfoArmor(GetAmor());
			PlayDamageIncreaseSound();
		}
		else {
			if (DamageIncreaseWidgetInstance)
			{
				DamageIncreaseWidgetInstance->ShowNotEnoughGoldMessage();
				PlayDamageIncreaseFailSound();
			}
		}
	}
}

void AProjectNo1Character::IncreaseDamage()
{
	// 공격력을 증가시키는 코드
	WeaponDamage += 100.0f;
	bDamageIncreaseState = false;
}

void AProjectNo1Character::RestoreDamage()
{
	// 공격력을 원래대로 복구시키는 코드
	// 예를 들어, CharacterDamage = BaseDamage;
	WeaponDamage -= 100.0f;
	bDamageIncreaseState = true;
}

void AProjectNo1Character::IncreaseSkillDamage()
{
	// 공격력을 증가시키는 코드
	WeaponDamage += 400.0f;
	bDamageIncreaseState = false;
}

void AProjectNo1Character::RestoreSkillDamage()
{
	// 공격력을 원래대로 복구시키는 코드
	// 예를 들어, CharacterDamage = BaseDamage;
	WeaponDamage -= 400.0f;
	bDamageIncreaseState = true;
}


void AProjectNo1Character::IncreaseStunDamage()
{
	// 공격력을 증가시키는 코드
	WeaponDamage += 500.0f;
	bDamageIncreaseState = false;
}

void AProjectNo1Character::RestoreStunDamage()
{
	// 공격력을 원래대로 복구시키는 코드
	// 예를 들어, CharacterDamage = BaseDamage;
	WeaponDamage -= 500.0f;
	bDamageIncreaseState = true;
}

void AProjectNo1Character::IncreaseCounterDamage()
{
	// 공격력을 증가시키는 코드
	WeaponDamage += 400.0f;
	bDamageIncreaseState = false;
}

void AProjectNo1Character::RestoreCounterDamage()
{
	// 공격력을 원래대로 복구시키는 코드
	// 예를 들어, CharacterDamage = BaseDamage;
	WeaponDamage -= 400.0f;
	bDamageIncreaseState = true;
}

void AProjectNo1Character::SetStatus()
{
	if (InfoWidgetInstance) {
		InfoWidgetInstance->SetHealth(Attributes->GetMaxHealth());
		InfoWidgetInstance->SetStamina(Attributes->GetMaxStamina());
		InfoWidgetInstance->SetHealthRegenRate(Attributes->GetHealthRegenRate());
		InfoWidgetInstance->SetStaminaRegenRate(Attributes->GetStaminaRegenRate());
		InfoWidgetInstance->SetExperience(Attributes->GetMaxExperience());
		InfoWidgetInstance->SetStaminaRegenRate(Attributes->GetStaminaRegenRate());
		InfoWidgetInstance->SetInfoDamage(GetWeaponDamage());
		InfoWidgetInstance->SetInfoArmor(GetAmor());
	}
	if (SlashOverlay) {
		SlashOverlay->SetGold(Attributes->GetGold());
		SlashOverlay->SetLevel(Attributes->GetLevel());
	}
}

void AProjectNo1Character::SetStatusWithDmgAm()
{
	if (InfoWidgetInstance) {
		InfoWidgetInstance->SetHealth(Attributes->GetMaxHealth());
		InfoWidgetInstance->SetStamina(Attributes->GetMaxStamina());
		InfoWidgetInstance->SetHealthRegenRate(Attributes->GetHealthRegenRate());
		InfoWidgetInstance->SetStaminaRegenRate(Attributes->GetStaminaRegenRate());
		InfoWidgetInstance->SetExperience(Attributes->GetMaxExperience());
		InfoWidgetInstance->SetStaminaRegenRate(Attributes->GetStaminaRegenRate());
		InfoWidgetInstance->SetInfoDamage(GetWeaponDamage());
		InfoWidgetInstance->SetInfoArmor(GetAmor());
	}
	if (SlashOverlay) {
		SlashOverlay->SetGold(Attributes->GetGold());
		SlashOverlay->SetLevel(Attributes->GetLevel());
	}
	if (DamageIncreaseWidgetInstance) {
		DamageIncreaseWidgetInstance->SetDamage(GetWeaponDamage());
		DamageIncreaseWidgetInstance->SetAmor(GetAmor());
	}
}

void AProjectNo1Character::CaveRegionOpen()
{
	if (SlashOverlay && MapWidgetInstance) {
		SlashOverlay->MapOpenTextMessage();
		MapWidgetInstance->MapCaveOpen();
	}
	bCaveState = true;
}

void AProjectNo1Character::IceLandRegionOpen()
{
	if (SlashOverlay && MapWidgetInstance) {
		SlashOverlay->MapOpenTextMessage();
		MapWidgetInstance->MapIceLandOpen();
	}
	bIceLandState = true;
}

void AProjectNo1Character::ForestRegionOpen()
{
	if (SlashOverlay && MapWidgetInstance) {
		SlashOverlay->MapOpenTextMessage();
		MapWidgetInstance->MapForestOpen();
	}
	bForestState = true;
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

	CheckForStunnedEnemy();
	CheckForNotStunnedEnemy();
	CheckForBackEnemy();
	CheckForNotBackEnemy();
}

void AProjectNo1Character::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(PlayerContext, 0);
		}
	}

	RageSkillEffect->DeactivateSystem(); // 초기에는 비활성화
	PotionSkillEffect->DeactivateSystem(); // 초기에는 비활성화
	Tags.Add(FName("EngageableTarget"));
	InitializeSlashOverlay();//기본 UI 설정
	SpawnDefaultWeapon();//주무기 장착
	SpawnDefaultWeaponTwo();//쉴드 장착
	SpawnDefaultPotionOne();//포션 장착
	CheckBossMonsters();//보스 체크
	InfoWidget();//인포 UI
	DamageIncreaseWidget();//데미지 증가 UI
	MapWidget();//맵 UI
	SystemWidget();//설정 UI
	AllMenuWidget();//전체 메뉴 UI
	BlessingPointSetting();
	SetStatusWithDmgAm();

	OnStunnedEnemyDetected.AddDynamic(this, &AProjectNo1Character::EnableSpecialTargetingAttack);
	OffStunnedEnemyDetected.AddDynamic(this, &AProjectNo1Character::DisableSpecialTargetingAttack);
	OnEnemyBackDetected.AddDynamic(this, &AProjectNo1Character::EnableSpecialTargetingAttack);
	OffEnemyBackDetected.AddDynamic(this, &AProjectNo1Character::DisableSpecialTargetingAttack);
}

void AProjectNo1Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AProjectNo1Character::Move);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AProjectNo1Character::Jump);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProjectNo1Character::Look);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AProjectNo1Character::Attack);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &AProjectNo1Character::EKeyPressed);
		EnhancedInputComponent->BindAction(DiveAction, ETriggerEvent::Triggered, this, &AProjectNo1Character::Dive);
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Triggered, this, &AProjectNo1Character::Parry);
		EnhancedInputComponent->BindAction(RageAction, ETriggerEvent::Triggered, this, &AProjectNo1Character::OnNeckSkillPressed);
		EnhancedInputComponent->BindAction(WeaponSpellAction, ETriggerEvent::Triggered, this, &AProjectNo1Character::WeaponSpellAttack);
		EnhancedInputComponent->BindAction(SwordSkillAction, ETriggerEvent::Triggered, this, &AProjectNo1Character::OnSwordSkillPressed);
		EnhancedInputComponent->BindAction(ExcuteAction, ETriggerEvent::Triggered, this, &AProjectNo1Character::SpecialTargetingAttackInput);
		EnhancedInputComponent->BindAction(GuardCounterAction, ETriggerEvent::Triggered, this, &AProjectNo1Character::GuardCounterPressed);
		EnhancedInputComponent->BindAction(BlessingInteractAction, ETriggerEvent::Triggered, this, &AProjectNo1Character::BlessingPointInteractInput);
		EnhancedInputComponent->BindAction(AllMenuAction, ETriggerEvent::Started, this, &AProjectNo1Character::ToggleAllMenuUI);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &AProjectNo1Character::RMKeyPressed);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &AProjectNo1Character::RMKeyReleased);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AProjectNo1Character::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AProjectNo1Character::StopSprinting);
	}
}

void AProjectNo1Character::Move(const FInputActionValue& value)
{
	if (Dead() || IsAttackSkill()) return; //공격중일때 이동 불가
	const FVector2D MovementVector = value.Get<FVector2D>();
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AProjectNo1Character::Look(const FInputActionValue& value)
{
	const FVector2D LookVector = value.Get<FVector2D>();
	if (Dead() || IsAttackSkill() || HitReact()) return;
	if (GetController()) {
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void AProjectNo1Character::Jump()
{
	if (!Dead() && IsUnoccupied())
	{
		Super::Jump();
	}
}

void AProjectNo1Character::Sprint()//달리기
{
	if (!Dead() && HasEnoughStamina() && ActionState == EActionState::EAS_Unoccupied) {
		GetCharacterMovement()->MaxWalkSpeed = 450.f;

		ActionState = EActionState::EAS_Sprint;
	}
}

void AProjectNo1Character::StopSprinting()
{
	if (ActionState == EActionState::EAS_Sprint) {
		GetCharacterMovement()->MaxWalkSpeed = 300.f;

		ActionState = EActionState::EAS_Unoccupied;
	}
}


void AProjectNo1Character::OnNeckSkillPressed()
{
	FTimerHandle NeckSkillCountdown;
	FTimerHandle RageEndTimerHandle;
	FTimerHandle RageEffectEndTimerHandle;

	if (!Dead() && CanNeckSkill() && HasEnoughSkillStamina() && bCanRage)
	{
		PlayNeckSkillMontage();
		ActionState = EActionState::EAS_NeckSkillDo;
		bCanRage = false;//연속 사용 불가
		IncreaseDamage();// 공격력 증가
		IncreaseSpellDamage();
		RageSkillEffect->ActivateSystem();//분노 이펙트 활성화
		GetWorldTimerManager().SetTimer(NeckSkillCountdown, this, &AProjectNo1Character::EnableRage, RageCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(RageEndTimerHandle, this, &AProjectNo1Character::RestoreAllDamage, RageDuration, false);//분노 지속시간 타이머 시작
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

void AProjectNo1Character::RestoreAllDamage()
{
	RestoreDamage();// 공격력 복구
	RestoreSpellDamage();
}

void AProjectNo1Character::IncreaseSpellDamage()
{
	// 공격력을 증가시키는 코드
	SpellDamage += 100.0f;
}

void AProjectNo1Character::RestoreSpellDamage()
{
	// 공격력을 원래대로 복구시키는 코드
	// 예를 들어, CharacterDamage = BaseDamage;
	SpellDamage -= 100.0f;
}

void AProjectNo1Character::EnableRage()
{
	bCanRage = true; // 쿨타임이 끝나면 다시 스킬을 사용할 수 있도록 설정
}

void AProjectNo1Character::LargeSkillPressed()
{
	FTimerHandle LargeSkillCountdown;

		if (!Dead() && CanNeckSkill() && HasEnoughSkillStamina() && bCanLargeSkill)
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
	if (SwordProjectileClass)
	{
		FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
		FRotator SpawnRotation = GetActorRotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		AProjectileWeapon* Spear = GetWorld()->SpawnActor<AProjectileWeapon>(SwordProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (Spear) {
			AActor* SpearTarget = FindClosestEnemy();
			if (SpearTarget)
			{
				FVector Direction = (SpearTarget->GetActorLocation() - SpawnLocation).GetSafeNormal();
				Spear->InitializeVelocity(Direction, SpearTarget);
			}
			else {
				Spear->InitializeVelocity(GetActorForwardVector(), nullptr);
			}
		}
	}
}

AActor* AProjectNo1Character::FindClosestEnemy()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), FoundActors);

	AActor* ClosestEnemy = nullptr;
	float MinDistance = MAX_FLT;

	for (AActor* Actor : FoundActors)
	{
		float Distance = FVector::Dist(Actor->GetActorLocation(), GetActorLocation());
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			ClosestEnemy = Actor;
		}
	}

	return ClosestEnemy;
}

void AProjectNo1Character::GuardCounterPressed()
{
	FTimerHandle GuardCounterTimer;
	FTimerHandle EnableHitTimer;
	FTimerHandle CounterDamageTimer;

	if (!Dead() && CanNeckSkill() && HasEnoughSkillStamina() && bIsGuardCounterAttackEnabled)
	{
		PlayGuardCounterMontage();
		ActionState = EActionState::EAS_AttackSkill;
		bIsGuardCountdownEnabled = false; //쿨타임 부울 변수
		bIsGuardCounterAttackEnabled = false;//연속 사용 X
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//카운터 공격 시 무적
		IncreaseCounterDamage();// 공격력 증가
		ExecuteHold();
		CounterFront();
		GetWorldTimerManager().SetTimer(GuardCounterTimer, this, &AProjectNo1Character::EnableGuardCountdown, GuardCounterCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(EnableHitTimer, this, &AProjectNo1Character::EnableHit, EnableHitCountdown, false); // 카운트다운 타이머 시작
		GetWorldTimerManager().SetTimer(CounterDamageTimer, this, &AProjectNo1Character::RestoreCounterDamage, DamagebackCountdown, false); // 데미지 복구 타이머 시작
		UE_LOG(LogTemp, Log, TEXT("GuardCounter"));
		if (Attributes && SlashOverlay)
		{
			Attributes->UseStamina(Attributes->GetSkillCost());
			SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}
	}
}

void AProjectNo1Character::CounterFront() 
{
	FVector LaunchVelocity = GetActorForwardVector() * 500.0f;
	LaunchCharacter(LaunchVelocity, true, true);
}

void AProjectNo1Character::DeactivateGuardCounterEffect()
{
	EquippedWeapon->DeactivateGuardCounterEffect(); // 카운터 공격 이펙트 비활성화
}

void AProjectNo1Character::ActivateGuardCounterEffect()
{
	EquippedWeapon->ActivateGuardCounterEffect(); // 카운터 공격 이펙트 활성화
}

void AProjectNo1Character::EnableGuardCountdown()
{
	bIsGuardCountdownEnabled = true;// 가드 카운터 공격 활성화
}

void AProjectNo1Character::EnableGuardCounter()
{
	FTimerHandle GuardCounterCountdown;
	if (bIsGuardCountdownEnabled) {
		bIsGuardCounterAttackEnabled = true;// 가드 카운터 활성화
		GetWorldTimerManager().SetTimer(GuardCounterCountdown, this, &AProjectNo1Character::DisableGuardCounter, GuardCounterDisableTimer, false);
		//일정 시간 내에 가드 카운터 공격하지 않을 시 비활성화
	}
}

void AProjectNo1Character::DisableGuardCounter()
{
	bIsGuardCounterAttackEnabled = false;// 가드 카운터 비활성화
}

void AProjectNo1Character::SmallSkillPressed()
{
	FTimerHandle SmallSkillCountdown;
	FTimerHandle EnableHitTimer;
	FTimerHandle StunDamageTimer;

	if (!Dead() && CanNeckSkill() && HasEnoughSkillStamina() && bCanSmallSkill)
	{
		PlaySmallSkillMontage();
		ActionState = EActionState::EAS_AttackSkill;
		bCanSmallSkill = false;//연속 사용 X
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//스턴 공격 시 무적
		IncreaseStunDamage();// 공격력 증가
		ExecuteHold();
		UE_LOG(LogTemp, Log, TEXT("StunAttack"));
		GetWorldTimerManager().SetTimer(SmallSkillCountdown, this, &AProjectNo1Character::EnableSmallSkill, SmallSkillCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(EnableHitTimer, this, &AProjectNo1Character::EnableHit, EnableHitCountdown, false); // 카운트다운 타이머 시작
		GetWorldTimerManager().SetTimer(StunDamageTimer, this, &AProjectNo1Character::RestoreStunDamage, DamagebackCountdown, false); // 카운트다운 타이머 시작
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
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제
}

void AProjectNo1Character::EnableSmallSkill()
{
	bCanSmallSkill = true;// 약공격 활성화
}

void AProjectNo1Character::EnableHit()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제
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

void AProjectNo1Character::ExecuteHold()
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

				if (LichEnemy)
				{
					LichEnemy->TakeExecutionHold();
				}
		    }
			else if (OverlappingActor->IsA(AGoblin::StaticClass()))
			{
				Goblin = Cast<AGoblin>(OverlappingActor);

				if (Goblin)
				{
					Goblin->TakeExecutionHold();
				}
			}
			else if (OverlappingActor->IsA(ACaveEnemy::StaticClass()))
			{
				CaveEnemy = Cast<ACaveEnemy>(OverlappingActor);

				if (CaveEnemy)
				{
					CaveEnemy->TakeExecutionHold();
				}
			}
	    }
    }
}

void AProjectNo1Character::PerformBack()
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
			if (OverlappingActor->IsA(ALichEnemy::StaticClass()))
			{
				LichEnemy = Cast<ALichEnemy>(OverlappingActor);
				if (LichEnemy)
				{
					LichEnemy->TakeBack();
				}
			}
			else if (OverlappingActor->IsA(AGoblin::StaticClass()))
			{
				Goblin = Cast<AGoblin>(OverlappingActor);
				if (Goblin)
				{
					Goblin->TakeBack();
				}
			}
			else if (OverlappingActor->IsA(ACaveEnemy::StaticClass()))
			{
				CaveEnemy = Cast<ACaveEnemy>(OverlappingActor);
				if (CaveEnemy)
				{
					CaveEnemy->TakeBack();
				}
			}
		}
	}
}

void AProjectNo1Character::ShowExecuteMessage()
{
	if (SlashOverlay) {
		SlashOverlay->ShowExecuteTextMessage();
		UE_LOG(LogTemp, Log, TEXT("ShowExecuteTextMessage")); 
	}
}

void AProjectNo1Character::HideExecuteMessage()
{
	if (SlashOverlay) {
		SlashOverlay->HideExecuteTextMessage();
		UE_LOG(LogTemp, Log, TEXT("HideExecuteTextMessage"));
	}
}

void AProjectNo1Character::Parry()
{
	FTimerHandle ParryCount;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!Dead() && CanAttack() && HasEnoughAttackStamina())
	{
		AnimInstance->Montage_Play(ParryMontage);
		bCanParry = false; // 패리 한 후 연속 패리 x
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
					ShowExecuteMessage();
					return true;
				}
				else if (LichEnemy && LichEnemy->IsNotStunned()) {
					//UE_LOG(LogTemp, Log, TEXT("LichEnemyNotStunnedCheck"));//확인완료
					HideExecuteMessage();
					return false;
				}
			}
		}
	}
	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 2.0f); //확인완료
	HideExecuteMessage();
	return false;
}

void AProjectNo1Character::CheckForStunnedEnemy()
{
	if (HasStunnedEnemyInFront())
	{
		// 델리게이트 호출
		OnStunnedEnemyDetected.Broadcast();
	}
}

void AProjectNo1Character::CheckForNotStunnedEnemy()
{
	if (!HasStunnedEnemyInFront())
	{
		// 델리게이트 호출
		OffStunnedEnemyDetected.Broadcast();
	}
}

bool AProjectNo1Character::HasEnemyBackInFront()//플레이어 앞 적 뒤잡 체크 함수
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

				if (LichEnemy)
				{
					FVector EnemyForward = LichEnemy->GetActorForwardVector();
					FVector ToPlayer = (StartLocation - LichEnemy->GetActorLocation()).GetSafeNormal();

					float DotProduct = FVector::DotProduct(EnemyForward, ToPlayer);
					if (DotProduct <= -0.5f)
					{
						//UE_LOG(LogTemp, Log, TEXT("LichEnemybehindCheck")); //확인완료
						ShowExecuteMessage();
						return true;
					}
					else {
						//UE_LOG(LogTemp, Log, TEXT("LichEnemyNotbehindCheck"));//확인완료
						HideExecuteMessage();
						return false;
					}
				}
			}
			else if (OverlappingActor->IsA(AGoblin::StaticClass()))
			{
				Goblin = Cast<AGoblin>(OverlappingActor);

				if (Goblin)
				{
					FVector EnemyForward = Goblin->GetActorForwardVector();
					FVector ToPlayer = (StartLocation - Goblin->GetActorLocation()).GetSafeNormal();

					float DotProduct = FVector::DotProduct(EnemyForward, ToPlayer);
					if (DotProduct <= -0.5f)
					{
						//UE_LOG(LogTemp, Log, TEXT("GoblinbehindCheck")); //확인완료
						ShowExecuteMessage();
						return true;
					}
					else {
						//UE_LOG(LogTemp, Log, TEXT("GoblinNotbehindCheck"));//확인완료
						HideExecuteMessage();
						return false;
					}
				}
			}
			else if (OverlappingActor->IsA(ACaveEnemy::StaticClass()))
			{
				CaveEnemy = Cast<ACaveEnemy>(OverlappingActor);

				if (CaveEnemy)
				{
					FVector EnemyForward = CaveEnemy->GetActorForwardVector();
					FVector ToPlayer = (StartLocation - CaveEnemy->GetActorLocation()).GetSafeNormal();

					float DotProduct = FVector::DotProduct(EnemyForward, ToPlayer);
					if (DotProduct <= -0.5f)
					{
						//UE_LOG(LogTemp, Log, TEXT("CaveEnemybehindCheck")); //확인완료
						ShowExecuteMessage();
						return true;
					}
					else {
						//UE_LOG(LogTemp, Log, TEXT("CaveEnemyNotbehindCheck"));//확인완료
						HideExecuteMessage();
						return false;
					}
				}
			}
		}
	}
	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 2.0f); //확인완료
	HideExecuteMessage();
	return false;
}

void AProjectNo1Character::CheckForBackEnemy()
{
	if (!HasStunnedEnemyInFront())
	{
		if (HasEnemyBackInFront())
		{
			// 델리게이트 호출
			OnEnemyBackDetected.Broadcast();
		}
	}
}

void AProjectNo1Character::CheckForNotBackEnemy()
{
	if (!HasStunnedEnemyInFront())
	{
		if (!HasEnemyBackInFront())
		{
			// 델리게이트 호출
			OffEnemyBackDetected.Broadcast();
		}
	}
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

void AProjectNo1Character::OnSwordSkillPressed()
{
	FTimerHandle SwordSkillCountdown;
	FTimerHandle SwordSkillAGITime;

	if (!Dead() && CanNeckSkill() && HasEnoughSkillStamina() && bCanSwordSkill)
	{
		PlaySwordSkillMontage();
		ActionState = EActionState::EAS_AttackSkill;
		bCanSwordSkill = false;//연속 사용 X
		IncreaseSkillDamage();// 공격력 증가
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
	RestoreSkillDamage(); // 공격력 복구
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

void AProjectNo1Character::WeaponSpellAttack()
{
	FTimerHandle WeaponSpellSkillCooldown;
	FTimerHandle WeaponSpellSkillCountdown;

	if (!Dead() && CanNeckSkill() && HasEnoughSkillStamina() && bCanWeaponSpell)
	{
		PlayWeaponSpellSkillMontage();
		ActionState = EActionState::EAS_WeaponSpell;
		bCanWeaponSpell = false;//연속 사용 불가
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//주문검 동작 시 무적
		GetWorldTimerManager().SetTimer(WeaponSpellSkillCooldown, this, &AProjectNo1Character::EnableWeaponSpell, WeaponSpellCooldown, false); // 쿨타임 타이머 시작
		GetWorldTimerManager().SetTimer(WeaponSpellSkillCountdown, this, &AProjectNo1Character::DeactivateWeaponSpellEffect, WeaponSpellCountdown, false); // 지속시간 타이머 시작

		if (Attributes && SlashOverlay)
		{
			Attributes->UseStamina(Attributes->GetSkillCost());
			SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}
	}
}
void AProjectNo1Character::WeaponSpellSlashEffect()
{
	FRotator PlayerRotation = GetActorRotation();
	FRotator DesiredRotation = PlayerRotation + FRotator(-45.0f, 100.0f, 45.0f);
	FVector ForwardVector = GetActorForwardVector(); // 플레이어 전방
	FVector SpawnLocationA = GetActorLocation() + (ForwardVector * 200.0f);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactEffect, SpawnLocationA, DesiredRotation);
}

void AProjectNo1Character::WeaponSpellLineTrace()
{
	if (AttackWeaponSpell) {
		FVector ForwardVector = GetActorForwardVector(); // 플레이어 전방
		FVector RightVector = GetActorRightVector(); // 플레이어 측면
		FVector Start = GetActorLocation() + (ForwardVector * 200.0f);// 플레이어 앞 위치에서 시작
		FVector End = Start + ForwardVector * 100.0f; // 일정 거리만큼 트레이스
		FRotator PlayerRotation = GetActorRotation();
		FVector SpawnLocationA = GetActorLocation() + (ForwardVector * 200.0f);
		FVector SpawnLocationB = SpawnLocationA + (RightVector * 80.0f) + (RightVector * 30.0f) + (ForwardVector * 30.0f);
		SpawnLocationB.Z += 100.0f;
		FVector SpawnLocationC = SpawnLocationA - (RightVector * 80.0f) + (RightVector * 30.0f) - (ForwardVector * 30.0f);
		SpawnLocationC.Z -= 100.0f;
		//TArray<FHitResult> HitResults;
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this); // 플레이어는 무시
		CollisionParams.OwnerTag = "EngageableTarget";
		bool bResult = GetWorld()->SweepSingleByChannel(
			HitResult,
			Start,
			End,
			FQuat::Identity,
			ECollisionChannel::ECC_Visibility,
			FCollisionShape::MakeSphere(50.0f),
			CollisionParams);

		// 트레이스의 시작점에 이펙트를 생성하여 표시
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EndPointEffect, SpawnLocationA, PlayerRotation);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SpellEffectA, SpawnLocationA, PlayerRotation);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SpellEffectB, SpawnLocationB, PlayerRotation);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SpellEffectC, SpawnLocationC, PlayerRotation);
		PlayWeaponSpellHitSound(SpawnLocationA);
		// 트레이스 실행
		if (bResult) {
			if (AActor* Actor = HitResult.GetActor()) {
				if (HitResult.GetActor()->ActorHasTag("Enemy"))
				{
					UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *Actor->GetName());
					FDamageEvent DamageEvent;

					ALichEnemy* HitEnemy = Cast<ALichEnemy>(HitResult.GetActor());
					AGoblin* GoblinEnemy = Cast<AGoblin>(HitResult.GetActor());
					ACaveEnemy* GhoulEnemy = Cast<ACaveEnemy>(HitResult.GetActor());
					ABossCharacter* BossCharacter = Cast<ABossCharacter>(HitResult.GetActor());

					Actor->SetOwner(this);
					Actor->SetInstigator(this);
					UGameplayStatics::ApplyDamage(HitResult.GetActor(), SpellDamage, GetInstigatorController(), this, UDamageType::StaticClass());
					ExecuteGetHit(HitResult);
					if (HitEnemy) {
						HitEnemy->ShowHitNumber(SpellDamage, HitResult.Location);
						HitEnemy->CombatTargetPlayer();
					}
					if (GoblinEnemy) {
						GoblinEnemy->ShowHitNumber(SpellDamage, HitResult.Location);
						GoblinEnemy->CombatTargetPlayer();
					}
					if (GhoulEnemy) {
						GhoulEnemy->ShowHitNumber(SpellDamage, HitResult.Location);
						GhoulEnemy->CombatTargetPlayer();
					}
					if (BossCharacter) {
						BossCharacter->ShowHitNumber(SpellDamage, HitResult.Location);
						BossCharacter->CombatTargetPlayer();
					}
				}
			}
		}
	}
}

void AProjectNo1Character::EnableWeaponSpell()
{
	bCanWeaponSpell = true;
}

void AProjectNo1Character::DeactivateWeaponSpellEffect()
{
	EquippedWeapon->DeactivateWeaponSpellEffect(); // 주문공격 이펙트 비활성화
	AttackWeaponSpell = false;
}

void AProjectNo1Character::ActivateWeaponSpellEffect()
{
	EquippedWeapon->ActivateWeaponSpellEffect(); // 주문공격 이펙트 활성화
	AttackWeaponSpell = true;//검기 활성화
}

void AProjectNo1Character::PlayWeaponSpellHitSound(const FVector& ImpactPoint)
{
	if (WeaponSpellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			WeaponSpellSound,
			ImpactPoint
		);
	}
}

void AProjectNo1Character::RMKeyPressed()
{
	if (!HasEnoughShieldStamina() || Dead() || BlockCantState()) return; //쉴드에 충분한 스태미너가 아니거나 달리기 상태, 스킬 사용 중일때 리턴
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

void AProjectNo1Character::BlockEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AProjectNo1Character::EndBlocking()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool AProjectNo1Character::BlockCantState()
{
	return ActionState == EActionState::EAS_AttackSkill ||
		ActionState == EActionState::EAS_WeaponSpell ||
		ActionState == EActionState::EAS_NeckSkillDo ||
		ActionState == EActionState::EAS_Attacking ||
		ActionState == EActionState::EAS_Parrying ||
		ActionState == EActionState::EAS_Sprint ||
		ActionState == EActionState::EAS_Drink ||
		ActionState == EActionState::EAS_LargeAttack ||
		ActionState == EActionState::EAS_Dive ||
		ActionState == EActionState::EAS_LevelUp ||
		ActionState == EActionState::EAS_HitReaction ||
		ActionState == EActionState::EAS_Dead;
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

void AProjectNo1Character::EquipNeck(AWeapon* NewNeck)
{
	EquippedNeck = NewNeck;
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

bool AProjectNo1Character::HitReact()
{
	return ActionState == EActionState::EAS_HitReaction;
}

void AProjectNo1Character::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
	UE_LOG(LogTemp, Log, TEXT("HitReactEnd"));
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

void AProjectNo1Character::SavePlayerState()
{
	UMyProGameInstance* GameInstance = Cast<UMyProGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance)
	{
		GameInstance->PlayerMaxHealth = Attributes->MaxHealth;
		GameInstance->PlayerMaxStamina = Attributes->MaxStamina;
		GameInstance->PlayerExperience = Attributes->Experience;
		GameInstance->PlayerMaxExperience = Attributes->MaxExperience;
		GameInstance->PlayerHealthRegenRate = Attributes->HealthRegenRate;
		GameInstance->PlayerStaminaRegenRate = Attributes->StaminaRegenRate;
		GameInstance->PlayerLevel = Attributes->Level;
		GameInstance->PlayerGold = Attributes->Gold;
		GameInstance->PlayerDamage = WeaponDamage;
		GameInstance->PlayerArmor = Amor;
		GameInstance->SavedBlessingPoint = LastBlessingPoint->GetActorLocation();
		GameInstance->bCaveState = bCaveState;
		GameInstance->bIceLandState = bIceLandState;
		GameInstance->bForestState = bForestState;
	}
	else
	{
		// 로그 추가
		UE_LOG(LogTemp, Error, TEXT("Failed to get SaveGameInstance"));
	}
	UE_LOG(LogTemp, Log, TEXT("SaveState"));
}

void AProjectNo1Character::LoadPlayerState()
{
	if (UMyProGameInstance* GameInstance = Cast<UMyProGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		Attributes->MaxHealth = GameInstance->PlayerMaxHealth;
		Attributes->MaxStamina = GameInstance->PlayerMaxStamina;
		Attributes->Experience = GameInstance->PlayerExperience;
		Attributes->MaxExperience = GameInstance->PlayerMaxExperience;
		Attributes->HealthRegenRate = GameInstance->PlayerHealthRegenRate;
		Attributes->StaminaRegenRate = GameInstance->PlayerStaminaRegenRate;
		Attributes->Level = GameInstance->PlayerLevel;
		Attributes->Gold = GameInstance->PlayerGold;
		WeaponDamage = GameInstance->PlayerDamage;
		Amor = GameInstance->PlayerArmor;
		BlessingPoint = GameInstance->SavedBlessingPoint; //축복 포인트 위치 저장
		SetActorLocation(BlessingPoint); //위치 로드
		bCaveState = GameInstance->bCaveState;
		bIceLandState = GameInstance->bIceLandState;
		bForestState = GameInstance->bForestState;
		MapLoadRegionOpen(); //지도 오픈 변수 로드
		Attributes->Health += Attributes->MaxHealth;
		Attributes->Stamina += Attributes->MaxStamina;
		UE_LOG(LogTemp, Log, TEXT("LoadState"));
	}
	else
	{
		// 로그 추가
		UE_LOG(LogTemp, Error, TEXT("Failed to get LoadGameInstance"));
	}
}

void AProjectNo1Character::MapLoadRegionOpen()
{
	if (MapWidgetInstance) {
		if (bCaveState) {
			MapWidgetInstance->MapCaveOpen();
		}
		if (bIceLandState) {
			MapWidgetInstance->MapIceLandOpen();
		}
		if (bForestState) {
			MapWidgetInstance->MapForestOpen();
		}
	}
}

void AProjectNo1Character::BlessingPointInteractInput()
{
	if (bInRangePoint() && bDamageIncreaseState) {
		bIsBlessingPointInteractEnabled = true;
		UE_LOG(LogTemp, Log, TEXT("BlessingPointInteractInput"));
	}
}

bool AProjectNo1Character::IsBlessingPointInteract()
{
	return bIsBlessingPointInteractEnabled == true;
}

bool AProjectNo1Character::bInRangePoint()
{
	return bInRange == true;
}

void AProjectNo1Character::bInRangePointTrue()
{
	bInRange = true;
}

void AProjectNo1Character::bInRangePointFalse()
{
	bInRange = false;
}

void AProjectNo1Character::BlessingPointInteract()
{
	UMyProGameInstance* GameInstance = Cast<UMyProGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
	AActor* PlayerStart = PlayerStarts[0];
		UE_LOG(LogTemp, Log, TEXT("BlessingPointInteract"));
		bIsBlessingPointInteractEnabled = false;
		PlayInteractMontage();
		ActionState = EActionState::EAS_Interact;
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//무적
		if (PlayerController) {
			PlayerController->SetIgnoreMoveInput(true);
			PlayerController->SetIgnoreLookInput(true);
		}
		GetCharacterMovement()->Deactivate();
		if (LastBlessingPoint)
		{
			UE_LOG(LogTemp, Log, TEXT("Interact"));
			Attributes->Health += Attributes->MaxHealth;
			SavePlayerState();
			if (GameInstance)
			{
				GameInstance->SaveGame();
			}
			PlayBlessInteractSound();
			if (PlayerStart)
			{
				// PlayerStart의 모빌리티 설정을 확인하고 Movable로 변경
				if (PlayerStart->GetRootComponent()->Mobility != EComponentMobility::Movable)
				{
					PlayerStart->GetRootComponent()->SetMobility(EComponentMobility::Movable);
				}
				if (PlayerStarts.Num() > 0)
				{
					UE_LOG(LogTemp, Log, TEXT("PlayerStartlocation"));
					PlayerStart->SetActorLocation(LastBlessingPoint->GetActorLocation()); //축복 지점으로 리스폰 장소 변경
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("LastBlessingPoint is nullptr"));
		}
}

void AProjectNo1Character::EndInteract()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	ActionState = EActionState::EAS_Unoccupied;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제
	GetCharacterMovement()->Activate();

	if (PlayerController) {
		PlayerController->SetIgnoreMoveInput(false);
		PlayerController->SetIgnoreLookInput(false);
	}
}

void AProjectNo1Character::BlessingPointSetting()
{
	TArray<AActor*> BlessingPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlessingPoint::StaticClass(), BlessingPoints);

	if (BlessingPoints.Num() > 0)
	{
		LastBlessingPoint = Cast<ABlessingPoint>(BlessingPoints[0]);
		UE_LOG(LogTemp, Log, TEXT("BlessingPointSetting is set to %s"), *LastBlessingPoint->GetName());
	}
}

void AProjectNo1Character::BlessingPointForestSetting()
{
	TArray<AActor*> BlessingPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlessingPoint::StaticClass(), BlessingPoints);

	if (BlessingPoints.Num() > 0)
	{
		LastBlessingPoint = Cast<ABlessingPoint>(BlessingPoints[0]);
		UE_LOG(LogTemp, Log, TEXT("BlessingPointForestSetting is Update to %s"), *LastBlessingPoint->GetName());
	}
}

void AProjectNo1Character::BlessingPointIceLandSetting()
{
	TArray<AActor*> BlessingPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlessingPoint::StaticClass(), BlessingPoints);

	if (BlessingPoints.Num() > 0)
	{
		LastBlessingPoint = Cast<ABlessingPoint>(BlessingPoints[1]);
		UE_LOG(LogTemp, Log, TEXT("BlessingPointIceLandSetting is Update to %s"), *LastBlessingPoint->GetName());
	}
}

void AProjectNo1Character::BlessingPointCaveSetting()
{
	TArray<AActor*> BlessingPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABlessingPoint::StaticClass(), BlessingPoints);

	if (BlessingPoints.Num() > 0)
	{
		LastBlessingPoint = Cast<ABlessingPoint>(BlessingPoints[2]);
		UE_LOG(LogTemp, Log, TEXT("BlessingPointCaveSetting is Update to %s"), *LastBlessingPoint->GetName());
	}
}

void AProjectNo1Character::ShowInteractMessage()
{
	if (SlashOverlay) {
		SlashOverlay->ShowInteractTextMessage();
	}
}

void AProjectNo1Character::HideInteractMessage()
{
	if (SlashOverlay) {
		SlashOverlay->HideInteractTextMessage();
	}
}

void AProjectNo1Character::IfAttack()
{
	if (!Dead() && ActionState == EActionState::EAS_Unoccupied)
	{
		Attack();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AProjectNo1Character::Attack()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	Super::Attack();
	
	if (bIsAllMenuUIVisible) return;
	if (!Dead() && CanAttack() && HasEnoughAttackStamina())
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
		if (AttackWeaponSpell) {
			WeaponSpellSlashEffect();
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

void AProjectNo1Character::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제
}

void AProjectNo1Character::EndAttacking()
{
	ActionState = EActionState::EAS_Unoccupied;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);//무적 해제
	GetCharacterMovement()->Activate();
}

void AProjectNo1Character::AttackSweepTrace()
{
		FVector ForwardVector = GetActorForwardVector(); // 플레이어 전방
		FVector RightVector = GetActorRightVector(); // 플레이어 측면
		FVector Start = GetActorLocation() + (ForwardVector *100.0f);
		FVector End = Start + ForwardVector * 100.0f; // 일정 거리만큼 트레이스
		FRotator PlayerRotation = GetActorRotation();
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this); // 플레이어는 무시
		CollisionParams.OwnerTag = "EngageableTarget";
		bool bResult = GetWorld()->SweepSingleByChannel(
			HitResult,
			Start,
			End,
			FQuat::Identity,
			ECollisionChannel::ECC_Visibility,
			FCollisionShape::MakeSphere(50.0f),
			CollisionParams);

		// 트레이스 실행
		if (bResult) {
				if (AActor* Actor = HitResult.GetActor()) {
					if (HitResult.GetActor()->ActorHasTag("Enemy"))
					{
						UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *Actor->GetName());
						FDamageEvent DamageEvent;

						ALichEnemy* HitEnemy = Cast<ALichEnemy>(HitResult.GetActor());
						AGoblin* GoblinEnemy = Cast<AGoblin>(HitResult.GetActor());
						ACaveEnemy* GhoulEnemy = Cast<ACaveEnemy>(HitResult.GetActor());
						ABossCharacter* BossCharacter = Cast<ABossCharacter>(HitResult.GetActor());

						Actor->SetOwner(this);
						Actor->SetInstigator(this);
						UGameplayStatics::ApplyDamage(HitResult.GetActor(), WeaponDamage, GetInstigatorController(), this, UDamageType::StaticClass());
						ExecuteGetHit(HitResult);
						if (HitEnemy) {
							HitEnemy->ShowHitNumber(WeaponDamage, HitResult.Location);
							HitEnemy->CombatTargetPlayer();
						}
						if (GoblinEnemy) {
							GoblinEnemy->ShowHitNumber(WeaponDamage, HitResult.Location);
							GoblinEnemy->CombatTargetPlayer();
						}
						if (GhoulEnemy) {
							GhoulEnemy->ShowHitNumber(WeaponDamage, HitResult.Location);
							GhoulEnemy->CombatTargetPlayer();
						}
						if (BossCharacter) {
							BossCharacter->ShowHitNumber(WeaponDamage, HitResult.Location);
							BossCharacter->CombatTargetPlayer();
						}
					}
				}
		}
}


void AProjectNo1Character::Dive()
{
	FTimerHandle DiveCountdown;

	if (!Dead() && CanAttack() && HasEnoughStamina() && bCanDive) {
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

	if (!Dead() && Unequipoccupied() && IsArm() && HasEnoughPotionStamina() && bCanDrinkPotion)
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

bool AProjectNo1Character::Unequipoccupied()//어느 행동도 하지않을 때
{
	return ActionState == EActionState::EAS_Unoccupied;
	
}

bool AProjectNo1Character::Dead()//죽은 상태
{
	return ActionState == EActionState::EAS_Dead ||
		bPlayerDead == true;
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

void AProjectNo1Character::Die()
{
	Super::Die();
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController) {
		PlayerController->SetIgnoreMoveInput(true);
		PlayerController->SetIgnoreLookInput(true);
		PlayerController->bShowMouseCursor = true;
		AMyProNo1HUD* MyProNo1HUD = Cast<AMyProNo1HUD>(PlayerController->GetHUD());
	}
	GetCharacterMovement()->Deactivate();
	UE_LOG(LogTemp, Log, TEXT("Dead"));
	ActionState = EActionState::EAS_Dead;
	bPlayerDead = true;
	DisableMeshCollision();
	PlayerDieUI();
}

void AProjectNo1Character::Respawn()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		AProjectNo1GameMode* GameMode = Cast<AProjectNo1GameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameMode)
		{
			GameMode->RestartPlayerAtPlayerStart(this);
			PlayGetUpMontage();
			GetCharacterMovement()->Activate();
			EnableMeshCollision();
			PlayerCanMove(); 
			PlayRespawnSound();
			bPlayerDead = false;
			ActionState = EActionState::EAS_Unoccupied;
			Tags.Remove(FName("Dead"));
			LoadPlayerState();
		}
	}
}

void AProjectNo1Character::PlayerDieUI()
{
	// 게임 종료 UI 생성
	UWorld* World = GetWorld();
	if (World)
	{
		if (World && GameRetryUIClass)
		{
			GameRetryUI = CreateWidget<UGameRetryUI>(World, GameRetryUIClass);
			GameRetryUI->AddToViewport();
		}
	}
}
void AProjectNo1Character::PlayerStartUI()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		AMyProNo1HUD* MyProNo1HUD = Cast<AMyProNo1HUD>(PlayerController->GetHUD());
		if (MyProNo1HUD)
		{
			MyProNo1HUD->PlayerStartUI();
			MyProNo1HUD->PlayerHideUI();
		}
	}
}

void AProjectNo1Character::ShowSlashOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		AMyProNo1HUD* MyProNo1HUD = Cast<AMyProNo1HUD>(PlayerController->GetHUD());
		if (MyProNo1HUD)
		{
			MyProNo1HUD->PlayerShowUI();
		}
	}
}

void AProjectNo1Character::HideSlashOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		AMyProNo1HUD* MyProNo1HUD = Cast<AMyProNo1HUD>(PlayerController->GetHUD());
		if (MyProNo1HUD)
		{
			MyProNo1HUD->PlayerHideUI();
		}
	}
}

void AProjectNo1Character::CheckBossMonsters()
{
	TArray<AActor*> FoundLichEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALichEnemy::StaticClass(), FoundLichEnemies);
	RemainingMonsters = FoundLichEnemies.Num();

	// 몬스터들이 모두 사라질 때를 감지하는 이벤트를 등록
	for (AActor* ChLichEnemy : FoundLichEnemies)
	{
		ALichEnemy* LichEnemyInstance = Cast<ALichEnemy>(ChLichEnemy);
		if (LichEnemyInstance)
		{
			RemainingMonsters--;
			if (RemainingMonsters <= 0)
			{
				LichEnemyInstance->OnBossDestroyedDelegate.AddDynamic(this, &AProjectNo1Character::GameClearUI);
			}
		}
	}
}

void AProjectNo1Character::GameClearUI()
{
	// 보스 처치 UI 생성
	FTimerHandle LevelTransitionTimerHandle;
	UWorld* World = GetWorld();
	if (World)
	{
		if (World && ClearWidgetClass)
		{
			ClearWidget = CreateWidget<UUserWidget>(World, ClearWidgetClass);
			ClearWidget->AddToViewport();
		}
		GetWorld()->GetTimerManager().SetTimer(LevelTransitionTimerHandle, this, &AProjectNo1Character::RemoveClearWidget, 1.0f, false);
	}
	if (Attributes && SlashOverlay)
	{
		Attributes->BossClearGold();
		SlashOverlay->SetGold(Attributes->GetGold());
	}
}

void AProjectNo1Character::RemoveClearWidget()
{
	if (ClearWidget)
	{
		ClearWidget->RemoveFromParent();
	}
}

void AProjectNo1Character::ToggleAllMenuUI()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (AllMenuWidgetInstance)
	{
		if (!bIsAllMenuUIVisible) {

			AllMenuWidgetInstance->Show();
			bIsAllMenuUIVisible = true;
			PlayAllMenuOpenSound();
			UE_LOG(LogTemp, Log, TEXT("Show"));
			if (PlayerController) {
				PlayerController->SetIgnoreMoveInput(true);
				PlayerController->SetIgnoreLookInput(true);
				PlayerController->bShowMouseCursor = true;
				GetCharacterMovement()->Deactivate();
			}
		}
		else {
			AllMenuWidgetInstance->Hide();
			if (InfoWidgetInstance) InfoWidgetInstance->Hide();
			if (MapWidgetInstance) MapWidgetInstance->Hide();
			if (DamageIncreaseWidgetInstance) DamageIncreaseWidgetInstance->Hide();
			if (SystemWidgetInstance) SystemWidgetInstance->Hide();
			bIsAllMenuUIVisible = false;
			PlayAllMenuCloseSound();
			UE_LOG(LogTemp, Log, TEXT("Hide"));
			if (PlayerController) {
				PlayerController->SetIgnoreMoveInput(false);
				PlayerController->SetIgnoreLookInput(false);
				PlayerController->bShowMouseCursor = false;
				GetCharacterMovement()->Activate();
			}
		}
	}
}

void AProjectNo1Character::AllMenuWidget()
{
	if (AllMenuWidgetClass)
	{
		AllMenuWidgetInstance = CreateWidget<UAllMenuWidget>(GetWorld(), AllMenuWidgetClass);
		if (AllMenuWidgetInstance)
		{
			AllMenuWidgetInstance->AddToViewport();
			AllMenuWidgetInstance->Hide();
		}
	}
}

void AProjectNo1Character::InfoWidget()
{
	if (InfoWidgetClass)
	{
		InfoWidgetInstance = CreateWidget<UInfoWidget>(GetWorld(), InfoWidgetClass);
		if (InfoWidgetInstance)
		{
			InfoWidgetInstance->AddToViewport();
			InfoWidgetInstance->Hide();
		}
	}
}

void AProjectNo1Character::OpenInfoWidget()
{
		if (InfoWidgetInstance)
		{
			InfoWidgetInstance->Show();
		}
}

void AProjectNo1Character::DamageIncreaseWidget()
{
	if (DamageIncreaseWidgetClass)
	{
		DamageIncreaseWidgetInstance = CreateWidget<UDamageIncreaseWidget>(GetWorld(), DamageIncreaseWidgetClass);
		if (DamageIncreaseWidgetInstance)
		{
			DamageIncreaseWidgetInstance->AddToViewport();
			DamageIncreaseWidgetInstance->Hide();
		}
	}
}

void AProjectNo1Character::OpenDamageIncreaseWidget()
{
		if (DamageIncreaseWidgetInstance)
		{
			DamageIncreaseWidgetInstance->Show();
		}
}

void AProjectNo1Character::MapWidget()
{
	if (MapWidgetClass)
	{
		MapWidgetInstance = CreateWidget<UMapWidget>(GetWorld(), MapWidgetClass);
		if (MapWidgetInstance)
		{
			MapWidgetInstance->AddToViewport();
			MapWidgetInstance->Hide();
		}
	}
}

void AProjectNo1Character::OpenMapWidget()
{
		if (MapWidgetInstance)
		{
			MapWidgetInstance->Show();
		}
}

void AProjectNo1Character::SystemWidget()
{
	if (SystemWidgetClass)
	{
		SystemWidgetInstance = CreateWidget<USystemWidget>(GetWorld(), SystemWidgetClass);
		if (SystemWidgetInstance)
		{
			SystemWidgetInstance->AddToViewport();
			SystemWidgetInstance->Hide();
		}
	}
}

void AProjectNo1Character::OpenSystemWidget()
{
	if (SystemWidgetInstance)
	{
		SystemWidgetInstance->Show();
	}
}

void AProjectNo1Character::PlayerCanMove()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController) {
		PlayerController->SetIgnoreMoveInput(false);
		PlayerController->SetIgnoreLookInput(false);
		PlayerController->bShowMouseCursor = false;
		GetCharacterMovement()->Activate();
	}
	bIsAllMenuUIVisible = false;
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

