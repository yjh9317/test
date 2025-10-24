#pragma once

UENUM(BlueprintType)
enum class EInputState : uint8
{
	Pressed,
	Released
};

UENUM(BlueprintType)
enum class EMouseButton : uint8
{
	NONE,
	Left,
	Right
};


UENUM(BlueprintType)
enum class EInputMethod : uint8
{
	RightClick,
	DragAndDrop
};
