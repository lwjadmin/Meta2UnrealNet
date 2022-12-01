#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

UINTERFACE(Blueprintable)
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};

class ITEMPLUGIN_API IItemInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void AddHeal(float HealVal);
	virtual void AddHeal_Implementation(float HealVal) {};

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void AddMag();
	virtual void AddMag_Implementation() {};
public:
};
