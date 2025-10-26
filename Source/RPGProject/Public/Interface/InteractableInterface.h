#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"



UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class RPGPROJECT_API IInteractableInterface
{
	GENERATED_BODY()

public:
	/**
	 * Interactable 객체 초기화
	 * 
	 * BeginPlay에서 호출되어 상호작용 시스템을 설정합니다.
	 * - InteractableComponent 참조 설정
	 * - 하이라이트 대상 메시 등록
	 * - UI 위젯 설정
	 */
	virtual void InitializeInteractable() {};

	/**
	 * [Server] 상호작용 실행 (Main Interaction Logic)
	 * 
	 * 서버에서 실제 상호작용 로직을 실행합니다.
	 * - 아이템 지급
	 * - 상태 변경
	 * - 게임 로직 처리
	 */
	virtual void ExecuteInteraction(AActor* Interactor) {};

	/**
	 * [Server] 상호작용 종료
	 * 
	 * 상호작용이 끝날 때 서버에서 호출됩니다.
	 * - Viewer 목록에서 제거
	 * - 리소스 정리
	 * - 상태 리셋
	 */ 

	virtual void EndInteraction(AActor* Interactor) {};

	/**
	 * 상호작용 제거
	 * 기본 구현: 아무것도 하지 않음 (자식 클래스에서 오버라이드)
	 */

	virtual void RemoveInteraction() {};

	/**
	 * [Client] 상호작용 종료 시 클라이언트 로직
	 * 
	 * 상호작용이 끝날 때 클라이언트에서 실행됩니다.
	 * - UI 닫기
	 * - 이펙트 종료
	 * - 사운드 중지
	*/
	virtual void ClientEndInteraction(AActor* Interactor) {};

	/**
	* [Client] 상호작용 준비 (Pre-Interaction)
	* 
	* 상호작용이 실제로 실행되기 전에 클라이언트에서 호출됩니다.
	* - 입력 타입 체크 (Single/Holding/Mashing)
	* - 진행 상황 UI 표시 준비
	* - 클라이언트 전용 준비 작업
	*/
	
	virtual void ClientPrepareInteraction() {};

	/**
	 * [Client] 상호작용 시작 시 클라이언트 로직
	 * 
	 * 상호작용이 시작될 때 클라이언트에서 실행됩니다.
	 * - 비주얼 이펙트 재생
	 * - 사운드 재생
	 * - UI 표시
	 */
	
	virtual void ClientBeginInteraction(AActor* Interactor) {};

	/**
	* 상호작용 가능 여부 체크
	* 
	* 현재 이 객체와 상호작용할 수 있는지 확인합니다.
	* - 조건 체크 (퀘스트 완료 여부, 인벤토리 공간 등)
	* - 쿨다운 체크
	* - 권한 체크
	*/
	
	virtual bool CanBeInteractedWith() {return true;}
};
