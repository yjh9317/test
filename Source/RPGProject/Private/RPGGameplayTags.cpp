#include "RPGGameplayTags.h"

namespace RPGGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Alive, "Character.State.Alive");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Dead, "Character.State.Dead");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Spawning, "Character.State.Spawning");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Despawning, "Character.State.Despawning");

	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Idle, "Character.State.Movement.Idle");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Walking, "Character.State.Movement.Walking");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Running, "Character.State.Movement.Running");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Sprinting, "Character.State.Movement.Sprinting");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Jumping, "Character.State.Movement.Jumping");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Falling, "Character.State.Movement.Falling");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Landing, "Character.State.Movement.Landing");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_HardLanding, "Character.State.Movement.HardLanding");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Climbing, "Character.State.Movement.Climbing");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Swimming, "Character.State.Movement.Swimming");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Flying, "Character.State.Movement.Flying");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Riding, "Character.State.Movement.Riding");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Gliding, "Character.State.Movement.Gliding");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Crouching, "Character.State.Movement.Crouching");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Movement_Rolling, "Character.State.Movement.Rolling");

	UE_DEFINE_GAMEPLAY_TAG(Character_State_Combat_None, "Character.State.Combat.None");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Combat_Attacking, "Character.State.Combat.Attacking");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Combat_Blocking, "Character.State.Combat.Blocking");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Combat_Parrying, "Character.State.Combat.Parrying");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Combat_Dodging, "Character.State.Combat.Dodging");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Combat_Knockback, "Character.State.Combat.Knockback");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Combat_Invulnerable, "Character.State.Combat.Invulnerable");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Combat_SuperArmor, "Character.State.Combat.SuperArmor");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Combat_Countering, "Character.State.Combat.Countering");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Combat_Finishing, "Character.State.Combat.Finishing");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Combat_Channeling, "Character.State.Combat.Channeling");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Combat_Casting, "Character.State.Combat.Casting");
	UE_DEFINE_GAMEPLAY_TAG(Character_State_Combat_Aiming, "Character.State.Combat.Aiming");

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

	UE_DEFINE_GAMEPLAY_TAG(Weapon_Property_OneHanded, "Weapon.Property.OneHanded");
	UE_DEFINE_GAMEPLAY_TAG(Weapon_Property_TwoHanded, "Weapon.Property.TwoHanded");

	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Head, "Equipment.Slot.Head");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Chest, "Equipment.Slot.Chest");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Legs, "Equipment.Slot.Legs");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Boots, "Equipment.Slot.Boots");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Gloves, "Equipment.Slot.Gloves");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Neck, "Equipment.Slot.Neck");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_MainHand, "Equipment.Slot.MainHand");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_OffHand, "Equipment.Slot.OffHand");

	UE_DEFINE_GAMEPLAY_TAG(Attribute_Vital_Health, "Attribute.Vital.Health");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Vital_MaxHealth, "Attribute.Vital.MaxHealth");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Vital_Mana, "Attribute.Vital.Mana");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Vital_MaxMana, "Attribute.Vital.MaxMana");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Vital_Stamina, "Attribute.Vital.Stamina");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Vital_MaxStamina, "Attribute.Vital.MaxStamina");

	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Strength, "Attribute.Primary.Strength");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Intelligence, "Attribute.Primary.Intelligence");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Dexterity, "Attribute.Primary.Dexterity");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Constitution, "Attribute.Primary.Constitution");

	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_AttackPower, "Attribute.Secondary.AttackPower");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_MagicPower, "Attribute.Secondary.MagicPower");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Defense, "Attribute.Secondary.Defense");

	UE_DEFINE_GAMEPLAY_TAG(QTE_Type_SingleKey, "QTE.Type.SingleKey");
}