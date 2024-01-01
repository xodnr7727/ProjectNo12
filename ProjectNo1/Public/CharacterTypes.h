#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon"),
	ECS_EquippedNeck UMETA(DisplayName = "EquippedNeck"),
	ECS_Block UMETA(DisplayName = "Blocking"),
	ECS_Idle UMETA(DisplayName = "IDLE")
};

UENUM(BlueprintType)
enum class ECharacterEquipState : uint8
{
	ECS_EquippedNeck UMETA(DisplayName = "EquippedNeck")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_HitReaction UMETA(DisplayName = "HitReaction"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Blocking UMETA(DisplayName = "Blocking"),
	EAS_EndBlocking UMETA(DisplayName = "EndBlocking"),
	EAS_NeckSkillDo UMETA(DisplayName = "NeckSkillDo"),
	EAS_SwordSkillDo UMETA(DisplayName = "SwordSkillDo"),
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon"),
	EAS_Dive UMETA(DisplayName = "Dive"),
	EAS_Parrying UMETA(DisplayName = "Parrying"),
	EAS_Drink UMETA(DisplayName = "Drink"),
	EAS_Sprint UMETA(DisplayName = "Sprint"),
	EAS_AttackSkill UMETA(DisplayName = "AttackSkill"),
	EAS_LargeAttack UMETA(DisplayName = "LargeAttack")
};

UENUM(BlueprintType)
enum EDeathPose
{
	EDP_Alive UMETA(DisplayName = "Alive"),
	EDP_Death UMETA(DisplayName = "Death"),

	EDP_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_NoState UMETA(DisplayName = "NoState"),

	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged"),
	EES_Stunned UMETA(DisplayName = "Stunned")

};

UENUM(BlueprintType)
enum ESlotType
{
	SLOT_Head UMETA(DisplayName = "Head"),
	SLOT_Male UMETA(DisplayName = "Male"),
	SLOT_Shoes UMETA(DisplayName = "Shoes"),
	SLOT_WeaponOne UMETA(DisplayName = "WeaponOne"),
	SLOT_WeaponTwo UMETA(DisplayName = "WeaponTwo"),
	SLOT_Necklace UMETA(DisplayName = "Necklace"),
	SLOT_RingOne UMETA(DisplayName = "RingOne"),
	SLOT_RingTwo UMETA(DisplayName = "RingTwo")
};
