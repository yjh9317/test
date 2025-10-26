#include "RPGGameplayTags.h"

namespace RPGGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Alive, "Character.State.Alive");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Dead, "Character.State.Dead");

	// Vital Stats
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Vital_Health, "Character.Stat.Vital.Health");
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Vital_MaxHealth, "Character.Stat.Vital.MaxHealth");
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Vital_Mana, "Character.Stat.Vital.Mana");
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Vital_MaxMana, "Character.Stat.Vital.MaxMana");
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Vital_Stamina, "Character.Stat.Vital.Stamina");
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Vital_MaxStamina, "Character.Stat.Vital.MaxStamina");

	// Primary Stats
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Primary_Strength, "Character.Stat.Attribute.Strength");
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Primary_Intelligence, "Character.Stat.Attribute.Intelligence");
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Primary_Dexterity, "Character.Stat.Attribute.Dexterity");
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Primary_Constitution, "Character.Stat.Attribute.Constitution");

	// Secondary Stats
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Secondary_AttackPower, "Character.Stat.Secondary.AttackPower");
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Secondary_MagicPower, "Character.Stat.Secondary.MagicPower");
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Secondary_Defense, "Character.Stat.Secondary.Defense");
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Secondary_MagicDefense, "Character.Stat.Secondary.MagicDefense");
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Secondary_CriticalChance, "Character.Stat.Secondary.CriticalChance");
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Secondary_CriticalDamage, "Character.Stat.Secondary.CriticalDamage");
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Secondary_AttackSpeed, "Character.Stat.Secondary.AttackSpeed");
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Secondary_MovementSpeed, "Character.Stat.Secondary.MovementSpeed");

	// Progression
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Progression_Level, "Character.Stat.Progression.Level");
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Progression_Experience, "Character.Stat.Progression.Experience");
	UE_DEFINE_GAMEPLAY_TAG(Character_Stat_Progression_SkillPoints, "Character.Stat.Progression.SkillPoints");
	


	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Idle, "Character.State.Movement.Idle");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Walking, "Character.State.Movement.Walking");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Sprinting, "Character.State.Movement.Sprinting");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Falling, "Character.State.Movement.Falling");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_HardLanding, "Character.State.Movement.HardLanding");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Gliding, "Character.State.Movement.Gliding");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Crouching, "Character.State.Movement.Crouching");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Rolling, "Character.State.Movement.Rolling");

	UE_DEFINE_GAMEPLAY_TAG(Character_State_Combat_None, "Character.State.Combat.None");

	UE_DEFINE_GAMEPLAY_TAG(Character_State_Buff_Strengthened, "Character.State.Buff.Strengthened");

	UE_DEFINE_GAMEPLAY_TAG(Character_State_DeBuff_Slowed, "Character.State.DeBuff.Slowed");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_DeBuff_Stunned, "Character.State.DeBuff.Stunned");

	UE_DEFINE_GAMEPLAY_TAG(Action_PrimaryAttack, "Action.PrimaryAttack");
	UE_DEFINE_GAMEPLAY_TAG(Action_SecondaryAttack, "Action.SecondaryAttack");
	UE_DEFINE_GAMEPLAY_TAG(Action_SpecialAttack, "Action.SpecialAttack");
	UE_DEFINE_GAMEPLAY_TAG(Action_HeavyAttack, "Action.HeavyAttack");
	UE_DEFINE_GAMEPLAY_TAG(Action_ChargedAttack, "Action.ChargedAttack");
	UE_DEFINE_GAMEPLAY_TAG(Action_ComboAttack, "Action.ComboAttack");
	UE_DEFINE_GAMEPLAY_TAG(Action_Jump, "Action.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Action_Dodge, "Action.Dodge");
	UE_DEFINE_GAMEPLAY_TAG(Action_Roll, "Action.Roll");
	UE_DEFINE_GAMEPLAY_TAG(Action_Sprint, "Action.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Action_Interact, "Action.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Action_Block, "Action.Block");
	UE_DEFINE_GAMEPLAY_TAG(Action_Parry, "Action.Parry");
	UE_DEFINE_GAMEPLAY_TAG(Action_Counter, "Action.Counter");

	UE_DEFINE_GAMEPLAY_TAG(Input_Move, "Input.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Look, "Input.Look");
	UE_DEFINE_GAMEPLAY_TAG(Input_Jump, "Input.Jump");

	UE_DEFINE_GAMEPLAY_TAG(Input_LeftCtrl, "Input.LeftCtrl");
	UE_DEFINE_GAMEPLAY_TAG(Input_LeftShift, "Input.LeftShift");
	UE_DEFINE_GAMEPLAY_TAG(Input_LeftClick, "Input.LeftClick");
	UE_DEFINE_GAMEPLAY_TAG(Input_RightClick, "Input.RightClick");
	UE_DEFINE_GAMEPLAY_TAG(Input_ClimbMove, "Input.ClimbMove");
	UE_DEFINE_GAMEPLAY_TAG(Input_ClimbHop, "Input.ClimbHop");
	UE_DEFINE_GAMEPLAY_TAG(Input_ClimbAction, "Input.ClimbAction");
	UE_DEFINE_GAMEPLAY_TAG(Input_Grapple, "Input.Grapple");
	UE_DEFINE_GAMEPLAY_TAG(Input_TargetLock, "Input.TargetLock");

	UE_DEFINE_GAMEPLAY_TAG(Input_Interact, "Input.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Input_Inventory, "Input.Inventory");
	UE_DEFINE_GAMEPLAY_TAG(Input_Attack_Primary, "Input.Attack.Primary");
	UE_DEFINE_GAMEPLAY_TAG(Input_Attack_Secondary, "Input.Attack.Secondary");
	UE_DEFINE_GAMEPLAY_TAG(Input_Dodge, "Input.Dodge");
	UE_DEFINE_GAMEPLAY_TAG(Input_Sprint, "Input.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Input_Block, "Input.Block");
	UE_DEFINE_GAMEPLAY_TAG(Input_Parry, "Input.Parry");

	UE_DEFINE_GAMEPLAY_TAG(Shared_Cooldown_Skill_Primary, "Shared.Cooldown.Skill.Primary");

	UE_DEFINE_GAMEPLAY_TAG(Weapon_Type_Melee_Sword, "Weapon.Type.Melee.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Type_Melee_GreatedSword, "Weapon.Type.Melee.GreatedSword");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Type_Ranged_Bow, "Weapon.Type.Ranged.Bow");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Type_Magic_Staff, "Weapon.Type.Magic.Staff");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Type_Shield, "Weapon.Type.Shield");

	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Head, "Equipment.Slot.Head");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Chest, "Equipment.Slot.Chest");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Legs, "Equipment.Slot.Legs");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Boots, "Equipment.Slot.Boots");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Gloves, "Equipment.Slot.Gloves");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Neck, "Equipment.Slot.Neck");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_MainHand, "Equipment.Slot.MainHand");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_OffHand, "Equipment.Slot.OffHand");
	
}