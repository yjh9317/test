
#include "UI/Menu/MainMenu.h"
#include "Item/ItemBase.h"
#include "Player/Character/MainCharacter.h"

void UMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	MainCharacter = Cast<AMainCharacter>(GetOwningPlayerPawn());
}

bool UMainMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	// const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);
	//
	// if(MainCharacter && ItemDragDrop->SourceItem)
	// {
	// 	MainCharacter->DropItem(ItemDragDrop->SourceItem,ItemDragDrop->SourceItem->Quantity);
	// 	return true;
	// }
	return false;
}
