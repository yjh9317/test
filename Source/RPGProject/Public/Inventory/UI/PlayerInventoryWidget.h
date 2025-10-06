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

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
public:
	UPlayerInventoryWidget(const FObjectInitializer& ObjectInitializer);
	void BindInventoryDelegates();

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

	//Interface
	virtual void OnSlotClicked(UUserWidget* SlotWidget) override;
	virtual void OnSlotFocused(UUserWidget* SlotWidget) override;
	virtual void OnSlotHovered(UUserWidget* SlotWidget) override;

	UFUNCTION()
	void SwitchInventoryPanel(EInventoryPanel NewPanel);
	
	UFUNCTION()
	void HighlightSlot(int32 SlotIndex);
		
	UFUNCTION()
	void RefreshInventory(EInventoryPanel Panel);

	UFUNCTION()
	void OnSortingFinished();

protected:
	void ResetSlotFocus();
	void SetActivePanel();
	void CreateAllInventoryPanels();
	void CreateInventoryPanel(EInventoryPanel Panel);
	
public:

    UPROPERTY(EditDefaultsOnly, Category = "Inventory|Classes")
    TSubclassOf<UPlayerInventorySlotWidget> InventorySlotWidgetClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Components")
    TObjectPtr<UInventoryCoreComponent> ReceiverInventoryComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Components")
    TObjectPtr<UInventoryComponent> InventoryComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Components")
    TObjectPtr<UEquipmentComponent> EquipmentComponent;

    // BindWidget
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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Layout")
    int32 RowPosition = 10;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Layout")
    int32 ColumnPosition = 10;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Layout", meta = (ClampMin = "1", ClampMax = "10"))
    int32 RowLength = 7;
    
    UPROPERTY(BlueprintReadWrite, Category = "Inventory|State")
    int32 FocusedSlot = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Titles")
    FText FirstPanelTitle = FText::FromString("Panel 1");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Titles")
    FText SecondPanelTitle = FText::FromString("Panel 2");
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Titles")
    FText ThirdPanelTitle = FText::FromString("Panel 3");
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Titles")
    FText FourthPanelTitle = FText::FromString("Panel 4");

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
    EInventoryPanel DefaultPanel = EInventoryPanel::P1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings", meta = (ClampMin = "1", ClampMax = "100"))
    int32 SampleSlotsToDisplay = 30;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Settings")
    FVector2D DraggedImageSize = FVector2D(100.0f, 100.0f);
	
};
