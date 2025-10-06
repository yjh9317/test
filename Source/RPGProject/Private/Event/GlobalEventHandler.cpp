// Fill out your copyright notice in the Description page of Project Settings.


#include "Event/GlobalEventHandler.h"
#include "GameplayTagContainer.h"
#include "Event/EventBase/RPGEventBase.h"
#include "Header/DebugHeader.h"

void UGlobalEventHandler::CallGlobalEventByClass(UObject* Publisher, TSubclassOf<URPGEventBase> EventClass,
                                                 UObject* Payload, const TArray<FString>& Metadata)
{
	if (!Publisher || !EventClass || EventClass == URPGEventBase::StaticClass())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: CallGlobalEventByClass() method has no valid input variables");
		return;
	}
	CallGlobalEvent(Publisher, Payload, Metadata, ListenerEventsByClass, EventClass);
}

void UGlobalEventHandler::CallGlobalEventByName(UObject* Publisher, FName EventName, UObject* Payload,
	const TArray<FString>& Metadata)
{
	if (!Publisher || EventName.IsNone())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: CallGlobalEventByName() method has no valid input variables");
		return;
	}
	CallGlobalEvent(Publisher, Payload, Metadata, ListenerEventsByName, EventName);
}

void UGlobalEventHandler::CallGlobalEventByGameplayTag(UObject* Publisher, FGameplayTag EventGameplayTag,
	UObject* Payload, const TArray<FString>& Metadata)
{
	if (!Publisher || EventGameplayTag.GetTagName().IsNone())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: CallGlobalEventByGameplayTag() method has no valid input variables");
		return;
	}
	CallGlobalEvent(Publisher, Payload, Metadata, ListenerEventsByGameplayTag, EventGameplayTag);
}

void UGlobalEventHandler::BindGlobalEventByClass(TSubclassOf<URPGEventBase> EventClass,
	const FRPGOnEventCalledSingle& Event)
{
	if (!EventClass || EventClass == URPGEventBase::StaticClass())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: BindGlobalEventByClass() method has no valid input variables");
		return;
	}
	ListenerEventsByClass.FindOrAdd(EventClass).AddUnique(Event);
}

void UGlobalEventHandler::BindGlobalEventByName(FName EventName, const FRPGOnEventCalledSingle& Event)
{
	if (EventName.IsNone())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: BindGlobalEventByName() method has no valid input variables");
		return;
	}
	ListenerEventsByName.FindOrAdd(EventName).AddUnique(Event);
}

void UGlobalEventHandler::BindGlobalEventByGameplayTag(FGameplayTag EventGameplayTag,
	const FRPGOnEventCalledSingle& Event)
{
	if (EventGameplayTag.GetTagName().IsNone())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: BindGlobalEventByGameplayTag() method has no valid input variables");
		return;
	}
	ListenerEventsByGameplayTag.FindOrAdd(EventGameplayTag).AddUnique(Event);
}

void UGlobalEventHandler::UnbindGlobalEventByClass(TSubclassOf<URPGEventBase> EventClass,
	const FRPGOnEventCalledSingle& Event)
{
	if (!EventClass || EventClass == URPGEventBase::StaticClass())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: UnbindGlobalEventByClass() method has no valid input variables");
		return;
	}
	UnbindGlobalEvent(Event, ListenerEventsByClass, EventClass);
}

void UGlobalEventHandler::UnbindGlobalEventByName(FName EventName, const FRPGOnEventCalledSingle& Event)
{
	if (EventName.IsNone())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: UnbindGlobalEventByName() method has no valid input variables");
		return;
	}
	UnbindGlobalEvent(Event, ListenerEventsByName, EventName);
}

void UGlobalEventHandler::UnbindGlobalEventByGameplayTag(FGameplayTag EventGameplayTag,
	const FRPGOnEventCalledSingle& Event)
{
	if (EventGameplayTag.GetTagName().IsNone())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: UnbindGlobalEventByGameplayTag() method has no valid input variables");
		return;
	}
	UnbindGlobalEvent(Event, ListenerEventsByGameplayTag, EventGameplayTag);
}

void UGlobalEventHandler::UnbindAllGlobalEventsByClass(UObject* Unsubscriber, TSubclassOf<URPGEventBase> EventClass)
{
	if (!Unsubscriber || !EventClass || EventClass == URPGEventBase::StaticClass())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: UnbindAllGlobalEventsByClass() method has no valid input variables");
		return;
	}
	UnbindAllGlobalEvents(Unsubscriber, ListenerEventsByClass, EventClass);
}

void UGlobalEventHandler::UnbindAllGlobalEventsByName(UObject* Unsubscriber, FName EventName)
{
	if (!Unsubscriber || EventName.IsNone())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: UnbindAllGlobalEventsByName() method has no valid input variables");
		return;
	}
	UnbindAllGlobalEvents(Unsubscriber, ListenerEventsByName, EventName);
}

void UGlobalEventHandler::UnbindAllGlobalEventsByGameplayTag(UObject* Unsubscriber, FGameplayTag EventGameplayTag)
{
	if (!Unsubscriber || EventGameplayTag.GetTagName().IsNone())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: UnbindAllGlobalEventsByGameplayTag() method has no valid input variables");
		return;
	}
	UnbindAllGlobalEvents(Unsubscriber, ListenerEventsByGameplayTag, EventGameplayTag);
}

void UGlobalEventHandler::GetAllActiveGlobalEventsClass(TArray<TSubclassOf<URPGEventBase>>& Events)
{
	ListenerEventsByClass.GenerateKeyArray(Events); //If code grows, move to template.
}

void UGlobalEventHandler::GetAllActiveGlobalEventNames(TArray<FName>& Events)
{
	ListenerEventsByName.GenerateKeyArray(Events); //If code grows, move to template.
}

void UGlobalEventHandler::GetAllActiveGlobalEventGameplayTag(TArray<FGameplayTag>& Events)
{
	ListenerEventsByGameplayTag.GenerateKeyArray(Events);
}

void UGlobalEventHandler::GetAllBoundObjectsToEventByClass(TSubclassOf<URPGEventBase> EventClass,
	TArray<UObject*>& Objects)
{
	if (!EventClass || EventClass == URPGEventBase::StaticClass())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: GetAllBoundObjectsByClass() method has no valid input variables");
		return;
	}
	GetAllBoundObjectsToEvent(OUT Objects, ListenerEventsByClass, EventClass);
}

void UGlobalEventHandler::GetAllBoundObjectsToEventByName(FName EventName, TArray<UObject*>& Objects)
{
	if (EventName.IsNone())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: GetAllBoundObjectsToEventByName() method has no valid input variables");
		return;
	}
	GetAllBoundObjectsToEvent(OUT Objects, ListenerEventsByName, EventName);
}

void UGlobalEventHandler::GetAllBoundObjectsToEventByGameplayTag(FGameplayTag EventGameplayTag,
	TArray<UObject*>& Objects)
{
	if (EventGameplayTag.GetTagName().IsNone())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: GetAllBoundObjectsToEventByGameplayTag() method has no valid input variables");
		return;
	}
	GetAllBoundObjectsToEvent(OUT Objects, ListenerEventsByGameplayTag, EventGameplayTag);
}

void UGlobalEventHandler::ClearGlobalEventByClass(TSubclassOf<URPGEventBase> EventClass, const bool bForceClear)
{
	if (!EventClass || EventClass == URPGEventBase::StaticClass())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: ClearGlobalEventByClass() method has no valid input variables");
		return;
	}
	ClearGlobalEvent(bForceClear, ListenerEventsByClass, EventClass);
}

void UGlobalEventHandler::ClearGlobalEventByName(FName EventName, const bool bForceClear)
{
	if (EventName.IsNone())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: ClearGlobalEventByName() method has no valid input variables");
		return;
	}
	ClearGlobalEvent(bForceClear, ListenerEventsByName, EventName);
}

void UGlobalEventHandler::ClearGlobalEventByGameplayTag(FGameplayTag EventGameplayTag, const bool bForceClear)
{
	if (EventGameplayTag.GetTagName().IsNone())
	{
		DebugHeader::LogWarning("UGlobalEventHandler: ClearGlobalEventByGameplayTag() method has no valid input variables");
		return;
	}
	ClearGlobalEvent(bForceClear, ListenerEventsByGameplayTag, EventGameplayTag);
}

void UGlobalEventHandler::ClearAllGlobalEventsClass(const bool bForceClear)
{
	TArray<TSubclassOf<URPGEventBase>> Events;
	GetAllActiveGlobalEventsClass(OUT Events);
	//Clear all events
	for (auto currentEvent : Events) //If code grows, move to template.
	{
		ClearGlobalEventByClass(currentEvent, bForceClear);
	}
}

void UGlobalEventHandler::ClearAllGlobalEventNames(const bool bForceClear)
{
	TArray<FName> Events;
	GetAllActiveGlobalEventNames(OUT Events);
	//Clear all events
	for (auto currentEvent : Events) //If code grows, move to template.
	{
		ClearGlobalEventByName(currentEvent, bForceClear);
	}
}

void UGlobalEventHandler::ClearAllGlobalEventGameplayTags(const bool bForceClear)
{
	TArray<FGameplayTag> Events;
	GetAllActiveGlobalEventGameplayTag(OUT Events);
	//Clear all events
	for (auto currentEvent : Events) //If code grows, move to template.
	{
		ClearGlobalEventByGameplayTag(currentEvent, bForceClear);
	}
}
