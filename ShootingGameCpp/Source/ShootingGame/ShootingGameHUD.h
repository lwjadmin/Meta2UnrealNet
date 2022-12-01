#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShootingGameHUD.generated.h"

/*----------------------------------------------------------------------------
** ���� �÷��̽�, Actor�� �����ñ⸦ �����ϱ� ���� �ڵ带 �ִ� ���� ���� �� ����. 
** Init ����
[0] GameModeBase
[1] GameStateBase
[2] PlayerController
[3] PlayerCharacter
[4] PlayerState
[5] PlayerHUD
[6] ��Ÿ
----------------------------------------------------------------------------*/

UCLASS()
class SHOOTINGGAME_API AShootingGameHUD : public AHUD
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UUserWidget> HudWidgetClass;
	UPROPERTY(BlueprintReadWrite)
	UUserWidget* HudWidget;
public:
	UFUNCTION(BlueprintNativeEvent)
	void OnUpdateMyHp(float CurrentHp, float MaxHp);
	void OnUpdateMyHp_Implementation(float CurrentHp, float MaxHp);

	UFUNCTION(BlueprintNativeEvent)
	void OnUpdateMyAmmo(int Ammo);
	void OnUpdateMyAmmo_Implementation(int Ammo);

	UFUNCTION(BlueprintNativeEvent)
	void OnUpdateMyMag(int Mag);
	void OnUpdateMyMag_Implementation(int Mag);
protected:
	virtual void BeginPlay() override;
	void BindPlayerState();
	FTimerHandle th_BindPlayerState;
};
