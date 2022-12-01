#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "WeaponBurst.generated.h"

UCLASS()
class SHOOTINGGAME_API AWeaponBurst : public AWeapon
{
	GENERATED_BODY()
public:
	virtual void PressTrigger_Implementation(bool bIsPressed) override;
	void BurstShooting();
	FTimerHandle th_BurstShooting;
};
