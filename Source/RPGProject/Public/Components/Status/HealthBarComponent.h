#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatusChanged,float,NewHealth);

UCLASS()
class RPGPROJECT_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	
	FOnStatusChanged HealthChange;
	
	void SetHealthPercent(float Percent);

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleAnywhere, Category= "HP")
	class UHealthBar* HealthBarWidget;
	
};
