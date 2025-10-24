// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Data/ItemDataStructs.h"
#include "ItemToolTipWidget.generated.h"

class UItemStatsSlotWidget;
class UVerticalBox;
class UUniformGridPanel;
class UEquipmentComponent;
class UHorizontalBox;
class UTextBlock;
class USizeBox;
class UBorder;
class UImage;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UItemToolTipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void OnPlayAnimation();
    
    void ShowComparisonToolTip();
    void HideComparisonToolTip();
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
    bool bIsCompareItem;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
    FItemData ItemData;

protected:
    virtual void NativeConstruct() override;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget") 
    TObjectPtr<UUniformGridPanel> UniformGridPanel_Component;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget") 
    TObjectPtr<UVerticalBox> VerticalBox_ItemStats;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UImage> Image_VerticalColorShift;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UImage> Image_Background;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UImage> Image_Item;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UBorder> Border_ItemInfo;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UBorder> Border_Equipped;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UTextBlock> TextBlock_ItemName;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UTextBlock> TextBlock_ItemType;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UTextBlock> TextBlock_ItemRarity;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<USizeBox> SizeBox_ItemName;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<USizeBox> SizeBox_Attributes;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UHorizontalBox> HorizontalBox_RequiredLevel;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UTextBlock> TextBlock_RequiredLevelText;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UTextBlock> TextBlock_RequiredLevel;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UHorizontalBox> HorizontalBox_Durability;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UTextBlock> TextBlock_DurabilityText;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UTextBlock> TextBlock_DurabilityValue;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UTextBlock> TextBlock_Description;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UTextBlock> TextBlock_AlreadyRead;

private:
    void SetItemImageAsync();
    void SetItemInfo();
    void BuildItemStats();
    void SetItemName();
    void SetItemType();
    void SetItemRarity();
    void SetItemRarityColor();
    void SetItemDescription();
    void SetItemWeight();
    void SetItemValue();
    void SetItemRequiredLevel();
    void SetItemRequiredLevelColor();
    void SetItemDurability();
    void SetItemDurabilityColor();
    void UpdateItemStatsComparison(UItemToolTipWidget* InComparedToolTip);
    void SetAlreadyReadVisibility();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class", meta=(AllowPrivateAccess="true"))
    TSubclassOf<UItemToolTipWidget> ItemToolTipWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class", meta=(AllowPrivateAccess="true"))
    TSubclassOf<UItemStatsSlotWidget> ItemStatsSlotWidgetClass;
    
    UPROPERTY(BlueprintReadOnly, Category="Component", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UEquipmentComponent> PlayerEquipment;
    
    UPROPERTY(meta=(AllowPrivateAccess="true"))
    TMap<FString, UItemStatsSlotWidget*> StatWidgets;
    
    UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UItemToolTipWidget> ComparisonToolTipWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default", meta=(AllowPrivateAccess="true"))
    FLinearColor TextGreenColor = FLinearColor{0.f, 0.508881f, 0.076185, 1.f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default", meta=(AllowPrivateAccess="true"))
    FLinearColor TextRedColor = FLinearColor{0.526042f, 0.f, 0.046692f, 1.f};
};

// UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
// TObjectPtr<UTextBlock> TextBlock_Weight;
//
// UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
// TObjectPtr<UTextBlock> TextBlock_Gold;