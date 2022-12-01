#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShootingGameHUD.generated.h"

/*----------------------------------------------------------------------------
** 게임 플레이시, Actor의 생성시기를 제어하기 위한 코드를 넣는 것이 좋을 것 같다. 
** Init 순서
[0] GameModeBase
[1] GameStateBase
[2] PlayerController
[3] PlayerCharacter
[4] PlayerState
[5] PlayerHUD
[6] 기타
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
