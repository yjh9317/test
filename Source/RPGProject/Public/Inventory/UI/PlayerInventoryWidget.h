// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/WidgetSlotListener.h"
#include "Inventory/Components/InventoryCoreComponent.h"
#include "Item/Data/ItemDataStructs.h"
#include "PlayerInventoryWidget.generated.h"

class UTextBlock;
class UScrollBox;
class UPlayerInventorySlotWidget;
class UInventoryCoreComponent;
class UInventoryComponent;
class UEquipmentComponent;
class UInventoryPanelButtonWidget;
class UWidgetSwitcher;
class UUniformGridPanel;
class UButton;

enum class EInventoryPanel;

UCLASS()
class RPGPROJECT_API UPlayerInventoryWidget : public UUserWidget, public IWidgetSlotListener
{
	GENERATED_BODY()

public:
    UPlayerInventoryWidget(const FObjectInitializer& ObjectInitializer);

    //~ Begin Public Blueprint API
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetFocusToSlot(int32 SlotIndex);
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    virtual void BuildInventorySlots(const TArray<FItemData>& Inventory, int32 Size, UUniformGridPanel* UniformGridPanel);
  
    UFUNCTION(BlueprintPure, Category = "Inventory")
    EInventoryPanel GetActivePanel() const;
    
    UFUNCTION(BlueprintPure, Category = "Inventory")
    EInventoryPanel GetEnumByIndex(int32 Index) const;
    
    UFUNCTION(BlueprintPure, Category = "Inventory")
    UUniformGridPanel* GetUniformGridFromPanel(EInventoryPanel InPanel) const;
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SwitchToNextInventoryPanel(bool ToTheRight);
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AssignCurrentlyFocusedSlot(UUserWidget* SlotWidget);
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetPanelTitle(EInventoryPanel InPanel);
 
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void DisplaySampleSlots(int32 Slots);
    
    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool IsAnyPopupActive();
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetReceiverInventory();
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void TryToUseAnItem(FItemData InItemData);
    //~ End Public Blueprint API
    
    //~ Begin IWidgetSlotListener Interface
    virtual void OnSlotClicked(UUserWidget* SlotWidget) override;
    virtual void OnSlotFocused(UUserWidget* SlotWidget) override;
    virtual void OnSlotHovered(UUserWidget* SlotWidget) override;
    //~ End IWidgetSlotListener Interface

protected:
    //~ Begin UUserWidget Overrides
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    //~ End UUserWidget Overrides

    //~ Begin Delegate Handlers
    UFUNCTION()
    void SwitchInventoryPanel(EInventoryPanel NewPanel);
    
    UFUNCTION()
    void HighlightSlot(int32 SlotIndex);
       
    UFUNCTION()
    void RefreshInventory(EInventoryPanel Panel);

    UFUNCTION()
    void OnSortingFinished();
    //~ End Delegate Handlers

    //~ Begin Bound Widget Components
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Widgets")
    TObjectPtr<UUniformGridPanel> UniformGridPanel_Slot1;
    
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Widgets")
    TObjectPtr<UUniformGridPanel> UniformGridPanel_Slot2;
    
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Widgets")
    TObjectPtr<UUniformGridPanel> UniformGridPanel_Slot3;
    
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Widgets")
    TObjectPtr<UUniformGridPanel> UniformGridPanel_Slot4;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Widgets")
    TObjectPtr<UInventoryPanelButtonWidget> Button_FirstPanel;
    
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Widgets")
    TObjectPtr<UInventoryPanelButtonWidget> Button_SecondPanel;
    
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Widgets")
    TObjectPtr<UInventoryPanelButtonWidget> Button_ThirdPanel;
    
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Widgets")
    TObjectPtr<UInventoryPanelButtonWidget> Button_FourthPanel;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Widgets")
    TObjectPtr<UScrollBox> ScrollBox_InventoryPanel;
    
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Widgets")
    TObjectPtr<UTextBlock> TextBlock_PanelName;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Widgets")
    TObjectPtr<UWidgetSwitcher> PanelSwitcher;
    //~ End Bound Widget Components

private:
    //~ Begin Internal Helper Functions
    void BindInventoryDelegates();
    void ResetSlotFocus();
    void SetActivePanel();
    void CreateAllInventoryPanels();
    void CreateInventoryPanel(EInventoryPanel Panel);
    //~ End Internal Helper Functions

    //~ Begin Configuration & State Properties
    UPROPERTY(EditDefaultsOnly, Category = "Inventory|Classes", meta=(AllowPrivateAccess="true"))
    TSubclassOf<UPlayerInventorySlotWidget> InventorySlotWidgetClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UInventoryCoreComponent> ReceiverInventoryComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UInventoryComponent> InventoryComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UEquipmentComponent> EquipmentComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Layout", meta=(AllowPrivateAccess="true"))
    int32 RowPosition = 10;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Layout", meta=(AllowPrivateAccess="true"))
    int32 ColumnPosition = 10;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Layout", meta = (ClampMin = "1", ClampMax = "10", AllowPrivateAccess="true"))
    int32 RowLength = 7;
    
    UPROPERTY(BlueprintReadWrite, Category = "Inventory|State", meta=(AllowPrivateAccess="true"))
    int32 FocusedSlot = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Titles", meta=(AllowPrivateAccess="true"))
    FText FirstPanelTitle = FText::FromString("Panel 1");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Titles", meta=(AllowPrivateAccess="true"))
    FText SecondPanelTitle = FText::FromString("Panel 2");
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Titles", meta=(AllowPrivateAccess="true"))
    FText ThirdPanelTitle = FText::FromString("Panel 3");
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Titles", meta=(AllowPrivateAccess="true"))
    FText FourthPanelTitle = FText::FromString("Panel 4");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings", meta=(AllowPrivateAccess="true"))
    EInventoryPanel DefaultPanel = EInventoryPanel::P1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings", meta = (ClampMin = "1", ClampMax = "100", AllowPrivateAccess="true"))
    int32 SampleSlotsToDisplay = 30;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings", meta=(AllowPrivateAccess="true"))
    FVector2D DraggedImageSize = FVector2D(100.0f, 100.0f);
    //~ End Configuration & State Properties
};
