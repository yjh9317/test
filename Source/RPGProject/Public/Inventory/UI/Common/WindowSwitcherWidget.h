// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/WidgetManager.h"
#include "WindowSwitcherWidget.generated.h"

class UTextBlock;
class UButton;
class UOverlay;
class UWidgetSwitcher;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UWindowSwitcherWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    UWindowSwitcherWidget(const FObjectInitializer& ObjectInitializer);

    UButton* GetButtonByWidgetType(EWidgetType Type) const;
    UOverlay* GetWidgetSwitcherPanel(EWidgetType Type) const;
    void OnHoverButton(EWidgetType Type);
    void OnUnHoverButton(EWidgetType Type);
    
protected:
    virtual void NativePreConstruct() override;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Main")
    TObjectPtr<UWidgetSwitcher> Switcher;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Main")
    TObjectPtr<UOverlay> Overlay_Main;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Main")
    TObjectPtr<UButton> Button_Inventory;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Main")
    TObjectPtr<UTextBlock> TextBlock_Inventory;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Main")
    TObjectPtr<UButton> Button_Quest;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Main")
    TObjectPtr<UTextBlock> TextBlock_Quest;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Main")
    TObjectPtr<UButton> Button_Abilities;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Main")
    TObjectPtr<UTextBlock> TextBlock_Abilities;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Main")
    TObjectPtr<UButton> Button_Map;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Main")
    TObjectPtr<UTextBlock> TextBlock_Map;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Vendor")
    TObjectPtr<UOverlay> Overlay_Vendor;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Vendor")
    TObjectPtr<UButton> Button_Vendor;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Vendor")
    TObjectPtr<UTextBlock> TextBlock_Vendor;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Vendor")
    TObjectPtr<UButton> Button_VendorEquipment;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Vendor")
    TObjectPtr<UTextBlock> TextBlock_VendorEquipment;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Storage")
    TObjectPtr<UOverlay> Overlay_Storage;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Storage")
    TObjectPtr<UButton> Button_Storage;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Storage")
    TObjectPtr<UTextBlock> TextBlock_Storage;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Storage")
    TObjectPtr<UButton> Button_StorageEquipment;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Storage")
    TObjectPtr<UTextBlock> TextBlock_StorageEquipment;

private:
    EWidgetType GetActiveTab() const;
    EWidgetType GetActiveWidget() const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default", meta=(AllowPrivateAccess = "true"))
    FLinearColor HoveredBorderColor = FLinearColor{0.666667f, 0.399634f, 0.f, 1.f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default", meta=(AllowPrivateAccess = "true"))
    FLinearColor UnHoveredBorderColor = FLinearColor{0.327778f, 0.327778f, 0.327778f, 1.f};
};