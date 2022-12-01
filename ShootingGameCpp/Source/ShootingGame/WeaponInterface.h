#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

//Blueprint 사용 가능 Weapon 인터페이스
UINTERFACE(Blueprintable)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

class SHOOTINGGAME_API IWeaponInterface
{
	GENERATED_BODY()
public:
	//--FIRE-----------------------------------------------------
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PressTrigger(bool bIsPressed);
	virtual void PressTrigger_Implementation(bool bIsPressed) {};
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void NotifyShoot();
	virtual void NotifyShoot_Implementation() {};
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void IsCanUse(bool& bCanUse);
	virtual void IsCanUse_Implementation(bool& bCanUse) {};

	//--RELOAD---------------------------------------------------
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PressReload();
	virtual void PressReload_Implementation() {};
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void NotifyReload();
	virtual void NotifyReload_Implementation() {};

	//--GRAB-----------------------------------------------------
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void AttachWeapon(ACharacter* Character);
	virtual void AttachWeapon_Implementation(ACharacter* Character) {};

	//--DROP-----------------------------------------------------
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void DetachWeapon(ACharacter* Character);
	virtual void DetachWeapon_Implementation(ACharacter* Character) {};
};
