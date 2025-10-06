#pragma once

#include "GameplayTagContainer.h"
#include "ActionData.generated.h"

UENUM(BlueprintType)
enum class EActionState : uint8
{
	Ready       UMETA(DisplayName = "Ready"),        // 액션을 시작할 수 있는 준비된 상태
	Active      UMETA(DisplayName = "Active"),       // 액션이 현재 활성화(실행)되어 있는 상태
	Cooldown    UMETA(DisplayName = "Cooldown"),     // 액션이 끝나고 재사용 대기시간에 있는 상태
	Cancelled   UMETA(DisplayName = "Cancelled"),    // 액션이 강제로 중단된 상태
	Failed      UMETA(DisplayName = "Failed")        // 액션 실행에 실패한 상태
};

UENUM(BlueprintType)
enum class EActionType : uint8
{
	Active      UMETA(DisplayName = "Active"),       // 직접 발동형 액션
	Passive     UMETA(DisplayName = "Passive"),      // 지속 효과형 액션
	Toggle      UMETA(DisplayName = "Toggle"),       // 토글형 액션
	Instant     UMETA(DisplayName = "Instant")       // 즉시 실행형 액션
};

UENUM(BlueprintType)
enum class EActionPriority : uint8
{
	Low         = 0,
	Normal      = 1,
	High        = 2,
	Critical    = 3     // 다른 모든 액션을 중단시킬 수 있음
};


UENUM(BlueprintType)
enum class EActionEvent : uint8
{
	NONE,
	// 기본 이벤트
	ComboWindowOpen,
	ComboWindowClose,
    
	// 이동 관련
	MovementCancelable,    // 이동으로 취소 가능
	MovementLocked,        // 이동 불가
	RotationLocked,        // 회전 불가
    
	// 액션 제어
	HyperArmor,           // 슈퍼아머 (피격 무시)
	Parryable,            // 패리 가능 구간
	CounterWindow,        // 카운터 가능 구간
    
	// 소울류 특화
	StaminaRegenBlock,    // 스태미나 회복 차단
	IFrameStart,          // 무적 프레임 시작
	IFrameEnd,            // 무적 프레임 종료
	WeaponTrailOn,        // 무기 이펙트 켜기
    
	// 종료 관련
	Interrupt,
	ActionComplete,       // 액션 완료
	ActionReset          // 액션 리셋
};
