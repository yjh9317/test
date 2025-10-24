#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"

namespace DebugHeader
{
	static void Print(const FString& Message)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::MakeRandomColor(), Message);
		}
	}

	static void Print(const FString& Message, const FColor& Color)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, Color, Message);
		}
	}

	static void Print(const FString& Message, const FColor& Color, float Time)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, Time, Color, Message);
		}
	}
	
	template<typename T>
	FString ToString(const T& Value)
	{
		return FString(TEXT("[Unsupported Type]"));
	}

	template<>
	inline FString ToString(const FString& Value)
	{
		return Value;
	}

	template<>
	inline FString ToString(const int32& Value)
	{
		return FString::Printf(TEXT("%d"), Value);
	}
	
	template<>
	inline FString ToString(const float& Value)
	{
		return FString::Printf(TEXT("%.2f"), Value);
	}

	template<>
	inline FString ToString(const bool& Value)
	{
		return Value ? TEXT("true") : TEXT("false");
	}

	template<>
	inline FString ToString(const FName& Value)
	{
		return Value.ToString();
	}

	template<>
	inline FString ToString(const FText& Value)
	{
		return Value.ToString();
	}

	template<>
	inline FString ToString(const FVector& Value)
	{
		return Value.ToString();
	}

	template<>
	inline FString ToString(const FRotator& Value)
	{
		return Value.ToString();
	}

	static FString FormatArgs()
	{
		return TEXT("");
	}
	
	template<typename T>
	static FString FormatArgs(T first)
	{
		return ToString(first);
	}
	
	template<typename T, typename... Args>
	static FString FormatArgs(T first, Args... rest)
	{
		return ToString(first) + TEXT(" ") + FormatArgs(rest...);
	}

	template<typename... Args>
	static void LogWithFormat(const FString& FormatString, Args... args)
	{
		FString LogMessage = FString::Printf(*FormatString, args...);
		UE_LOG(LogTemp, Log, TEXT("%s"), *LogMessage);
	}

	template<typename... Args>
	static void Log(const FString& Message, Args... args)
	{
		FString LogMessage = Message + TEXT(" ") + FormatArgs(args...);
		UE_LOG(LogTemp, Log, TEXT("%s"), *LogMessage);
	}
	
	template<typename... Args>
	static void LogWarning(const FString& Message, Args... args)
	{
		FString LogMessage = Message + TEXT(" ") + FormatArgs(args...);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *LogMessage);
	}

	template<typename... Args>
	static void LogError(const FString& Message, Args... args)
	{
		FString LogMessage = Message + TEXT(" ") + FormatArgs(args...);
		UE_LOG(LogTemp, Error, TEXT("%s"), *LogMessage);
	}

	template<typename... Args>
	static void LogDisplay(const FString& Message, Args... args)
	{
		FString LogMessage = Message + TEXT(" ") + FormatArgs(args...);
		UE_LOG(LogTemp, Display, TEXT("%s"), *LogMessage);
	}

	static void DrawDebugLine(UWorld* World, const FVector& Start, const FVector& End, const FColor& Color,
	                          float Duration = 0.0f)
	{
		if (World)
		{
			DrawDebugLine(World, Start, End, Color, false, Duration);
		}
	}

	static void DrawDebugBox(UWorld* World, const FVector& Center, const FVector& Extent, const FColor& Color,
	                         float Duration = 0.0f)
	{
		if (World)
		{
			DrawDebugBox(World, Center, Extent, FQuat::Identity, Color, false, Duration);
		}
	}

	static void DrawDebugSphere(UWorld* World, const FVector& Center, float Radius, const FColor& Color,
	                            float Duration = 0.0f)
	{
		if (World)
		{
			DrawDebugSphere(World, Center, Radius, 12, Color, false, Duration);
		}
	}

	static void Assert(bool bCondition, const FString& ErrorMessage)
	{
		if (!bCondition)
		{
			UE_LOG(LogTemp, Error, TEXT("Assertion Failed: %s"), *ErrorMessage);
			check(false); // Trigger a breakpoint
		}
	}

	static void MeasureExecutionTime(TFunction<void()> FunctionToMeasure)
	{
		double StartTime = FPlatformTime::Seconds();
		FunctionToMeasure();
		double EndTime = FPlatformTime::Seconds();
		double ElapsedTime = EndTime - StartTime;
		UE_LOG(LogTemp, Log, TEXT("Execution Time: %f seconds"), ElapsedTime);
	}

	static void LogVector(const FVector& Vector)
	{
		UE_LOG(LogTemp, Log, TEXT("Vector: X=%f, Y=%f, Z=%f"), Vector.X, Vector.Y, Vector.Z);
	}

	static void EnableDebugCamera(APlayerController* PlayerController, bool bEnable)
	{
		if (PlayerController)
		{
			if (bEnable)
			{
				PlayerController->ConsoleCommand(TEXT("ToggleDebugCamera"), true);
			}
			else
			{
				PlayerController->ConsoleCommand(TEXT("ViewMode Lit"), true);
			}
		}
	}
	
	static void DrawDebugText(UWorld* World, const FVector& Location, const FString& Text, const FColor& Color,
	                          float Duration = 0.0f)
	{
		if (World)
		{
			DrawDebugString(World, Location, Text, nullptr, Color, Duration, false);
		}
	}
}
