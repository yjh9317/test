#pragma once

template<typename T>
FString EnumToString(const T EnumValue)
{
	const UEnum* EnumPtr = StaticEnum<T>();
	if (!EnumPtr)
	{
		return FString("Invalid");
	}

	return EnumPtr->GetNameStringByValue(static_cast<int64>(EnumValue));
}

template<typename T>
int32 GetEnumCount()
{
	const UEnum* EnumPtr = StaticEnum<T>();
	if (!EnumPtr)
	{
		return 0;
	}

	// 열거형 요소의 총 개수 반환
	return EnumPtr->NumEnums() - 1;  // 마지막 요소는 "_MAX" 또는 "Invalid"일 수 있으므로 하나 빼줌
}

UENUM(BlueprintType)
enum class EEnemyAIState : uint8
{
	EAS_Passive UMETA(Display = Passive),
	EAS_Patrol UMETA(Display = Patrolling),
	EAS_Chase UMETA(Display = Chase),
	EAS_Combat UMETA(Display = Combat),
	EAS_Dead UMETA(Display = Dead),
	
	EAS_MAX UMETA(Display = MAX)
};


UENUM(BlueprintType)
enum class EEnemyAISense : uint8
{
	EES_NONE  UMETA(Display = None),
	EES_Sight  UMETA(Display = Sight),
	EES_Hearing  UMETA(Display = Hearing),
	EES_Damage  UMETA(Display = Damage),

	EES_MAX UMETA(Display = MAX)
};

UENUM(BlueprintType)
enum class EEnemyMovementSpeed : uint8
{
	EMS_Idle UMETA(Display = Idle),
	EMS_Walking UMETA(Display = Walking), 
	EMS_Sprinting UMETA(Display = Sprinting),
};

