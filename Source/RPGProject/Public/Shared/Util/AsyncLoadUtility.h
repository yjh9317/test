#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h" // UAssetManager::Get() 사용을 위해


namespace RPGAsyncLoadUtil
{
template<typename AssetType>
static TSharedPtr<FStreamableHandle> RequestAsyncLoadObject(
    const TSoftObjectPtr<AssetType>& ObjectPtrToLoad,
    TFunction<void(AssetType*)> OnLoadedCallback
)
{
    if (ObjectPtrToLoad.IsNull())
    {
       if (OnLoadedCallback) OnLoadedCallback(nullptr);
       return nullptr;
    }

    FSoftObjectPath PathToLoad = ObjectPtrToLoad.ToSoftObjectPath();
    FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();

    TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(
       PathToLoad,
       [ObjectPtrToLoad, OnLoadedCallback, PathToLoad]() mutable { // ObjectPtrToLoad를 캡처
         AssetType* LoadedAsset = ObjectPtrToLoad.Get(); // TSoftObjectPtr<AssetType>::Get() 호출
         if (!LoadedAsset)
         {
           UE_LOG(LogTemp, Warning, TEXT("RequestAsyncLoadObject: Failed to load asset %s or wrong type for AssetType."), *PathToLoad.ToString());
         }
         if (OnLoadedCallback)
         {
           OnLoadedCallback(LoadedAsset);
         }
       }
    );
    return Handle;
}

/**
 * UClass 에셋을 비동기적으로 로드합니다.
 * @tparam ClassBaseType TSoftClassPtr이 가리키는 클래스의 기본 타입 (예: UItemBase, AActor)
 * @param ClassPtrToLoad 로드할 UClass에 대한 TSoftClassPtr
 * @param OnLoadedCallback 로드 완료 시 호출될 콜백 (UClass* 전달)
 */
template<typename ClassBaseType>
static TSharedPtr<FStreamableHandle> RequestAsyncLoadClass(
    const TSoftClassPtr<ClassBaseType>& ClassPtrToLoad,
    TFunction<void(UClass*)> OnLoadedCallback
)
{
    if (ClassPtrToLoad.IsNull())
    {
       if (OnLoadedCallback) OnLoadedCallback(nullptr);
       return nullptr;
    }

    FSoftObjectPath PathToLoad = ClassPtrToLoad.ToSoftObjectPath();
    // UClass*를 안전하게 얻기 위해 TSoftObjectPtr<UClass>를 생성하여 사용
    TSoftObjectPtr<UClass> InternalClassObjectPtr(PathToLoad);

    FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();

    TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(
       PathToLoad,
       [InternalClassObjectPtr, OnLoadedCallback, PathToLoad]() mutable { // TSoftObjectPtr<UClass>를 캡처
         UClass* LoadedClass = InternalClassObjectPtr.Get(); // TSoftObjectPtr<UClass>::Get() 호출
         if (!LoadedClass)
         {
           UE_LOG(LogTemp, Warning, TEXT("RequestAsyncLoadClass: Failed to load UClass %s or it's not a UClass type."), *PathToLoad.ToString());
         }
         // 선택적: 로드된 클래스가 ClassBaseType의 자식인지 확인 (TSoftClassPtr이 이미 강제하므로 중복일 수 있음)
         // if (LoadedClass && !LoadedClass->IsChildOf(ClassBaseType::StaticClass())) { UE_LOG(...) LoadedClass = nullptr; }

         if (OnLoadedCallback)
         {
           OnLoadedCallback(LoadedClass);
         }
       }
    );
    return Handle;
}

    template<typename AssetType>
    static TSharedPtr<FStreamableHandle> RequestAsyncLoadMultiple(
        const TArray<TSoftObjectPtr<AssetType>>& AssetsToLoad,
        TFunction<void(bool /* bAllSucceeded */, const TArray<AssetType*>& /* LoadedAssets */)> OnAllLoadedCallback
    )
    {
        if (AssetsToLoad.Num() == 0)
        {
            if (OnAllLoadedCallback) OnAllLoadedCallback(true, {}); // 성공, 빈 배열
            return nullptr;
        }

        TArray<FSoftObjectPath> PathsToLoad;
        for (const TSoftObjectPtr<AssetType>& AssetPtr : AssetsToLoad)
        {
            // Null이 아닌 경로만 추가 (주의: 이 경우 AssetsToLoad와 최종 LoadedAssets 배열의 인덱스가 달라질 수 있음)
            // 모든 경로를 유지하고 싶다면 AssetPtr.IsNull() 검사 없이 ToSoftObjectPath() 호출
            if (!AssetPtr.IsNull())
            {
                PathsToLoad.Add(AssetPtr.ToSoftObjectPath());
            }
        }

        if (PathsToLoad.Num() == 0) // 모든 TSoftObjectPtr이 Null이었던 경우
        {
             if (OnAllLoadedCallback) OnAllLoadedCallback(true, {});
             return nullptr;
        }

        FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();

        TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(
            PathsToLoad, // FStreamableManager는 경로 배열을 받아 처리 가능
            [AssetsToLoad, OnAllLoadedCallback]() mutable { // 원본 AssetsToLoad 배열을 캡처하여 순서대로 결과 생성
                TArray<AssetType*> LoadedAssets;
                LoadedAssets.Reserve(AssetsToLoad.Num());
                bool bAllSucceeded = true;

                for (const TSoftObjectPtr<AssetType>& AssetPtr : AssetsToLoad)
                {
                    if (AssetPtr.IsNull()) // 원본 배열에서 Null이었던 항목
                    {
                        LoadedAssets.Add(nullptr); // 결과 배열에도 nullptr 추가하여 인덱스 유지
                        continue;
                    }
                    
                    AssetType* LoadedAsset = AssetPtr.Get();
                    if (LoadedAsset)
                    {
                        LoadedAssets.Add(LoadedAsset);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("RequestAsyncLoadMultiple: Failed to load asset %s or wrong type."), *AssetPtr.ToString());
                        LoadedAssets.Add(nullptr); // 실패 시에도 nullptr 추가
                        bAllSucceeded = false;
                    }
                }

                if (OnAllLoadedCallback)
                {
                    OnAllLoadedCallback(bAllSucceeded, LoadedAssets);
                }
            }
        );
        return Handle; // 호출자가 핸들 관리
    }

	    template<typename CDOBaseType> // 예: UItemBase
    static TSharedPtr<FStreamableHandle> RequestAsyncLoadClassAndGetCDO(
        const TSoftClassPtr<CDOBaseType>& ClassToLoad,
        TFunction<void(CDOBaseType* /* CDO */)> OnCDOLoadedCallback
    )
    {
        if (ClassToLoad.IsNull())
        {
            if (OnCDOLoadedCallback) OnCDOLoadedCallback(nullptr);
            return nullptr;
        }

        // 이미 클래스가 로드되어 있는지 확인
        // ClassToLoad.Get()은 로드되지 않았으면 nullptr을 반환하며, 동기 로드를 유발하지 않습니다.
        UClass* PreLoadedClass = ClassToLoad.Get();
        if (PreLoadedClass)
        {
            CDOBaseType* CDO = Cast<CDOBaseType>(PreLoadedClass->GetDefaultObject());
            if (OnCDOLoadedCallback) OnCDOLoadedCallback(CDO);
            // 이미 로드된 경우, 비동기 작업이 시작되지 않았으므로 nullptr 반환
            return nullptr;
        }

        FSoftObjectPath PathToLoad = ClassToLoad.ToSoftObjectPath();
        FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();

        TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(
            PathToLoad, // UClass 에셋을 로드
            [ClassToLoad, OnCDOLoadedCallback, PathToLoad]() mutable {
                // 로드 완료 후 TSoftClassPtr에서 UClass*를 가져옴
                UClass* LoadedUClass = ClassToLoad.Get();
                CDOBaseType* CDO = nullptr;

                if (LoadedUClass)
                {
                    CDO = Cast<CDOBaseType>(LoadedUClass->GetDefaultObject());
                    if (!CDO)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("RequestAsyncLoadClassAndGetCDO: Loaded class %s, but failed to get/cast CDO of type %s."),
                               *LoadedUClass->GetName(), *CDOBaseType::StaticClass()->GetName());
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("RequestAsyncLoadClassAndGetCDO: Failed to load class asset for path: %s."), *PathToLoad.ToString());
                }

                if (OnCDOLoadedCallback)
                {
                    OnCDOLoadedCallback(CDO);
                }
            }
        );
        return Handle; // 호출자가 핸들 관리
    }

	template<typename AssetType, typename PayloadType>
   static TSharedPtr<FStreamableHandle> RequestAsyncLoadWithPayload(
	   const TSoftObjectPtr<AssetType>& AssetToLoad,
	   PayloadType UserPayload, // 페이로드는 값 또는 const 참조로 전달 (타입 및 사용 사례에 따라 결정)
	   TFunction<void(AssetType*, PayloadType)> OnLoadedCallbackWithPayload
   )
	{
		if (AssetToLoad.IsNull())
		{
			if (OnLoadedCallbackWithPayload) OnLoadedCallbackWithPayload(nullptr, UserPayload);
			return nullptr;
		}

		FSoftObjectPath PathToLoad = AssetToLoad.ToSoftObjectPath();
		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();

		// UserPayload가 복사 비용이 크다면 const&로 받고, 람다 캡처 시에도 주의 필요 (생명주기)
		// 여기서는 간단하게 값으로 캡처한다고 가정
		TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(
			PathToLoad,
			[AssetToLoad, UserPayload, OnLoadedCallbackWithPayload, PathToLoad]() mutable {
				AssetType* LoadedAsset = AssetToLoad.Get();
				if (!LoadedAsset)
				{
					UE_LOG(LogTemp, Warning, TEXT("RequestAsyncLoadWithPayload: Failed to load asset %s or wrong type."), *PathToLoad.ToString());
				}
				if (OnLoadedCallbackWithPayload)
				{
					OnLoadedCallbackWithPayload(LoadedAsset, UserPayload);
				}
			}
		);
		return Handle; // 호출자가 핸들 관리
	}
}