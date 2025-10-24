#pragma once

#include "CoreMinimal.h"
#include "PostProcessDefinitions.generated.h"

// UENUM(BlueprintType)
// enum class EPP_BlendModes : uint8
// {
// 	Normal UMETA(DisplayName = "Normal"),
// 	ColorBurn UMETA(DisplayName = "Color Burn"),
// 	ColorDodge UMETA(DisplayName = "Color Dodge"),
// 	Difference UMETA(DisplayName = "Difference"),
// 	Exclusion UMETA(DisplayName = "Exclusion"),
// 	Hardlight UMETA(DisplayName = "Hard light"),
// 	LinearBurn UMETA(DisplayName = "Linear Burn"),
// 	LinearDodge UMETA(DisplayName = "Linear Dodge"),
// 	LinearLight UMETA(DisplayName = "Linear Light"),
// 	PinLight UMETA(DisplayName = "PinLight"),
// 	SoftLight UMETA(DisplayName = "SoftLight"),
// 	Screen UMETA(DisplayName = "Screen"),
// 	Lighten UMETA(DisplayName = "Lighten"),
// 	Darken UMETA(DisplayName = "Darken"),
// 	Overlay UMETA(DisplayName = "Overlay"),
// 	Multiply UMETA(DisplayName = "Multiply")
// };
//
// USTRUCT(BlueprintType)
// struct FPP_EffectFeatures
// {
// 	GENERATED_BODY()
// public:
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	EPP_BlendModes PP_BlendModes;
// 	
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	TObjectPtr<UTexture2D> BlendMask;
// 	
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	FVector2D BlendMaskScale;
// 	
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	float BlendDistance;
// 	
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	float BlendDistanceSharpness;
// 	
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	bool BlendDistanceInvert;
// 	
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	float BlendingOpacity;
// 	
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	int32 EffectPriority;
//
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	bool CustomDepth;
//
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	bool InvertTheCustomDepthMasking;
//
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	bool StencilBuffer;
// 	
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	int32 StencilMask;
// };
//
// USTRUCT(BlueprintType)
// struct FPP_EffectFeaturesNoCustomDepth
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	EPP_BlendModes PP_BlendModes;
// 	
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	TObjectPtr<UTexture2D> BlendMask;
// 	
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	FVector2D BlendMaskScale;
// 	
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	float BlendDistance;
// 	
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	float BlendDistanceSharpness;
// 	
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	bool BlendDistanceInvert;
// 	
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	float BlendingOpacity;
// 	
// 	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
// 	int32 EffectPriority;
// };