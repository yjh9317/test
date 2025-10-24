// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemDragAndDropWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UItemDragAndDropWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default", meta=(ExposeOnSpawn="true"))
	TObjectPtr<UTexture2D> DraggableImage;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
	TObjectPtr<UImage> Image_WrongSlot;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
	TObjectPtr<UImage> Image_SwapSlot;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
	TObjectPtr<UImage> Image_DropSlot;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
	TObjectPtr<UImage> Image_Draggable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Default")
	FVector2D DraggableImageSize{100.f, 100.f};
protected:
	virtual void NativeConstruct() override;
};