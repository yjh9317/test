// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/Items/ToolTip/ItemToolTipWidget.h"

#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Status/StatsComponent.h"
#include "Interface/ComponentManager.h"
#include "Inventory/Components/EquipmentComponent.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Inventory/UI/Common/ItemStatsSlotWidget.h"

void UItemToolTipWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	APlayerController* LocalPC = GetOwningPlayer();
	
	if(LocalPC)
	{
		if(IComponentManager* ComponentManager = Cast<IComponentManager>(LocalPC))
		{
			PlayerEquipment = ComponentManager->GetEquipmentComponent();
		}
	}
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		SetVisibility(bIsCompareItem ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
		OnPlayAnimation();
		SetItemInfo();
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UItemToolTipWidget::SetItemInfo()
{
	SetItemName();
	SetItemType();
	SetItemRarity();
	SetItemDescription();
	SetItemRarityColor();
	BuildItemStats();
	SetItemWeight();
	SetItemValue();
	SetItemRequiredLevel();
	SetItemRequiredLevelColor();
	SetItemDurability();
	SetItemDurabilityColor();
	SetAlreadyReadVisibility();
}

void UItemToolTipWidget::ShowComparisonToolTip()
{
	APlayerController* LocalPC = GetOwningPlayer();
	if(!LocalPC)
	{
		return;
	}
	
	FItemData LocalSlotData;
	if(!ComparisonToolTipWidget && PlayerEquipment->GetItemByEquipmentSlot(ItemData.EquipmentSlot,LocalSlotData))
	{
		ComparisonToolTipWidget = CreateWidget<UItemToolTipWidget>(LocalPC,ItemToolTipWidgetClass);
		if(ComparisonToolTipWidget)
		{
			ComparisonToolTipWidget->ItemData = LocalSlotData;
			ComparisonToolTipWidget->bIsCompareItem = true;
		}
		if(UniformGridPanel_Component)
		{
			UUniformGridSlot* LocalGridSlot = UniformGridPanel_Component->AddChildToUniformGrid(ComparisonToolTipWidget,0,0);
			LocalGridSlot->SetColumn(-1); // Position on the left side
			LocalGridSlot->SetHorizontalAlignment(HAlign_Right);
			// Update Compared Values in THIS widget
			UpdateItemStatsComparison(ComparisonToolTipWidget);
			// Update Compared Values in COMPARED widget
			ComparisonToolTipWidget->UpdateItemStatsComparison(this);
		}
	}
}

void UItemToolTipWidget::HideComparisonToolTip()
{
	if(ComparisonToolTipWidget)
	{
		ComparisonToolTipWidget->RemoveFromParent();
		ComparisonToolTipWidget = nullptr;
		UpdateItemStatsComparison(nullptr);
	}
}

void UItemToolTipWidget::SetItemImageAsync()
{
	UItemBase* ItemBase = ItemData.ItemClass.GetDefaultObject();
	if(!ItemBase && ItemBase->Image.IsNull())
	{
		return;
	}

	// 2. 스트리밍 관리자를 통해 비동기 로딩을 요청합니다.
	// UAssetManager::GetStreamableManager()는 에셋을 관리하는 싱글톤 객체를 가져옵니다.
	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
    
	// RequestAsyncLoad 함수에 로드할 에셋 경로와, 로드가 완료되면 실행할 람다 함수를 전달합니다.
	StreamableManager.RequestAsyncLoad(ItemBase->Image.ToSoftObjectPath(), 
		// [this]는 이 람다 함수가 클래스 멤버(ItemImage)에 접근할 수 있게 합니다.
		[this, ItemBase]() 
		{
			// 💡 이 람다 함수 내부가 블루프린트의 'Completed' 핀 뒤에 연결된 로직과 같습니다.

			// 3. 로딩이 완료되면, 해당 에셋을 가져와서 이미지 위젯에 적용합니다.
			// 위젯이 파괴되었을 수도 있으므로 IsValid 체크를 하는 것이 안전합니다.
			if (IsValid(this) && IsValid(Image_Item))
			{
				// Get() 함수로 실제 UTexture2D* 객체를 가져옵니다.
				UTexture2D* LoadedTexture = Cast<UTexture2D>(ItemBase->Image.Get());
				if (LoadedTexture)
				{
					Image_Item->SetBrushFromTexture(LoadedTexture);
				}
			}
		}
	);
}

void UItemToolTipWidget::BuildItemStats()
{
	APlayerController* LocalPC = GetOwningPlayer();
	if(!UInventoryUtilityLibrary::IsItemClassValid(ItemData) || !ItemStatsSlotWidgetClass || !LocalPC)
	{ 
		return;
	}
	UItemBase* ItemBase = Cast<UItemBase>(ItemData.ItemClass->GetDefaultObject());
	
	if (!ItemBase) 
	{
		return;
	}
	StatWidgets.Empty();
	VerticalBox_ItemStats->ClearChildren();

	for(const auto& ItemStatRow : ItemBase->Stats)
	{
		UItemStatsSlotWidget* LocalItemStatsSlotWidget = CreateWidget<UItemStatsSlotWidget>(LocalPC,ItemStatsSlotWidgetClass);
		if(LocalItemStatsSlotWidget)
		{
			LocalItemStatsSlotWidget->StatName = FText::FromString(ItemStatRow.StatTag.ToString());
			LocalItemStatsSlotWidget->StatValue = ItemStatRow.Value;
			StatWidgets.Add(ItemStatRow.StatTag.ToString(),LocalItemStatsSlotWidget);

			UVerticalBoxSlot* LocalVerticalBoxSlot = VerticalBox_ItemStats->AddChildToVerticalBox(LocalItemStatsSlotWidget);
			LocalVerticalBoxSlot->SetHorizontalAlignment(HAlign_Fill);
		}
	}
}

void UItemToolTipWidget::SetItemName()
{
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		UItemBase* ItemBase = ItemData.ItemClass.GetDefaultObject();
		TextBlock_ItemName->SetText(ItemBase->Name);
	}
}

void UItemToolTipWidget::SetItemType()
{
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		UItemBase* ItemBase = ItemData.ItemClass.GetDefaultObject();
		
		const UEnum* TypeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EItemType"), true);
		if(TypeEnum)
		{
			FName EnumName = TypeEnum->GetNameByValue((int64)ItemBase->Type);
			TextBlock_ItemType->SetText(FText::FromName(EnumName));	
		}
	}
}

void UItemToolTipWidget::SetItemRarity()
{
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		UItemBase* ItemBase = ItemData.ItemClass.GetDefaultObject();
		
		const UEnum* TypeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EItemRarity"), true);
		if(TypeEnum)
		{
			FName EnumName = TypeEnum->GetNameByValue((int64)ItemBase->Rarity);
			TextBlock_ItemRarity->SetText(FText::FromName(EnumName));	
		}
	}
}

void UItemToolTipWidget::SetItemRarityColor()
{
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		UItemBase* ItemBase = ItemData.ItemClass.GetDefaultObject();
		FLinearColor RarityLinearColor = UInventoryUtilityLibrary::GetRarityColor(ItemBase->Rarity).GetSpecifiedColor();
		Border_ItemInfo->SetContentColorAndOpacity(RarityLinearColor);
		Border_ItemInfo->SetBrushColor(RarityLinearColor);
	}
}

void UItemToolTipWidget::SetItemDescription()
{
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		UItemBase* ItemBase = ItemData.ItemClass.GetDefaultObject();
		FText ItemDescription = ItemBase->Description;
		if(ItemDescription.IsEmpty())
		{
			TextBlock_Description->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			TextBlock_Description->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			TextBlock_Description->SetText(ItemDescription);
		}
	}
}

void UItemToolTipWidget::SetItemWeight()
{
	// Weight 
}

void UItemToolTipWidget::SetItemValue()
{
	// Value
}

void UItemToolTipWidget::SetItemRequiredLevel()
{
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		UItemBase* ItemBase = ItemData.ItemClass.GetDefaultObject();
		if(ItemBase->RequiredLevel > 0)
		{
			HorizontalBox_RequiredLevel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			TextBlock_RequiredLevel->SetText(FText::AsNumber(ItemBase->RequiredLevel));
		}
		else
		{
			HorizontalBox_RequiredLevel->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UItemToolTipWidget::SetItemRequiredLevelColor()
{
	APlayerController* LocalPC = GetOwningPlayer();
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData) || LocalPC)
	{
		IComponentManager* ComponentManager = Cast<IComponentManager>(LocalPC);
		if(ComponentManager)
		{
			UStatsComponent* StatsComponent = ComponentManager->GetStatsComponent();
			if(StatsComponent)
			{
				UItemBase* ItemBase = ItemData.ItemClass.GetDefaultObject();
				if(StatsComponent->CurrentLevel >= ItemBase->RequiredLevel)
				{
					TextBlock_RequiredLevel->SetColorAndOpacity(TextGreenColor);
					TextBlock_RequiredLevelText->SetColorAndOpacity(TextGreenColor);
				}
				else
				{
					TextBlock_RequiredLevel->SetColorAndOpacity(TextRedColor);
					TextBlock_RequiredLevelText->SetColorAndOpacity(TextRedColor);
				}
			}
		}
	}
}

void UItemToolTipWidget::SetItemDurability()
{
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		UItemBase* ItemBase = ItemData.ItemClass.GetDefaultObject();
		if(ItemBase->UseDurability)
		{
			HorizontalBox_Durability->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			const int32 DurabilityValue = ItemData.Durability;
			FFormatOrderedArguments Args;
			Args.Add(FText::AsNumber(DurabilityValue));
			FText FormattedText = FText::Format(NSLOCTEXT("GameUI", "DurabilityFormat", "{0}%"), Args);
			TextBlock_DurabilityValue->SetText(FormattedText);
		}
		else
		{
			HorizontalBox_Durability->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UItemToolTipWidget::SetItemDurabilityColor()
{
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		
		FLinearColor FinalColor = ItemData.Durability > 10 ? TextGreenColor : TextRedColor;
		TextBlock_DurabilityText->SetColorAndOpacity(FinalColor);
		TextBlock_DurabilityValue->SetColorAndOpacity(FinalColor);
	}
}

void UItemToolTipWidget::UpdateItemStatsComparison(UItemToolTipWidget* InComparedToolTip)
{
	TArray<FString> Keys;
	StatWidgets.GenerateKeyArray(Keys);
	
	if(InComparedToolTip)
	{
		for(const auto& Key : Keys)
		{
			UItemStatsSlotWidget* ComparedStatsSlotWidget = InComparedToolTip->StatWidgets[Key];
			UItemStatsSlotWidget* StatsSlotWidget = StatWidgets[Key];
			if(ComparedStatsSlotWidget)
			{
				StatsSlotWidget->ShowComparisonToolTip(ComparedStatsSlotWidget->StatValue);
			}
			else
			{
				StatsSlotWidget->ShowComparisonToolTip(0.f);
			}
		}
	}
	else
	{
		for(const auto& Key : Keys)
		{
			StatWidgets[Key]->HideComparisonToolTip();
		}
	}
}

void UItemToolTipWidget::SetAlreadyReadVisibility()
{
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		UItemBase* ItemBase = ItemData.ItemClass.GetDefaultObject();
		const bool IsVisible = ItemBase->UseType == EItemUseType::TextDocument && ItemData.AlreadyUsed;
		TextBlock_AlreadyRead->SetVisibility(IsVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
}
