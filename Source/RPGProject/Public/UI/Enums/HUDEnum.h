#pragma once

#include "CoreMinimal.h"
#include "HUDEnum.generated.h"

UENUM()
enum class ERotationType
{
	ControlRotation,
	CameraRotation
};


UENUM()
enum class EWidgetRenderTransform
{
	Translation,
	Scale,
	Shear
};

UENUM()
enum class ETaskState
{
	InActive,
	Completed,
	Failed,
	Active
};

UENUM()
enum class ETaskRemoveState
{
	Completed,
	Failed,
	JustRemove
};
