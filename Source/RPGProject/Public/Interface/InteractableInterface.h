#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"



UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class RPGPROJECT_API IInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void Initialize();
	virtual void Initialize_Implementation();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void EndInteraction(AActor* Interactor);
	virtual void EndInteraction_Implementation(AActor* Interactor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void Interaction(AActor* Interactor);
	virtual void Interaction_Implementation(AActor* Interactor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void RemoveInteraction();
	virtual void RemoveInteraction_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void ClientStartInteraction(AActor* Interactor);
	virtual void ClientStartInteraction_Implementation(AActor* Interactor);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void ClientEndInteraction(AActor* Interactor);
	virtual void ClientEndInteraction_Implementation(AActor* Interactor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void ClientPreInteraction(AActor* Interactor);
	virtual void ClientPreInteraction_Implementation(AActor* Interactor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	bool CanBeInteractedWith();
	virtual bool CanBeInteractedWith_Implementation() {return true;}
};
