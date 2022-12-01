#include "ShootingGameHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "ShootingPlayerState.h"

void AShootingGameHUD::OnUpdateMyHp_Implementation(float CurrentHp, float MaxHp)
{
	//블루프린트에서 호출!
}

void AShootingGameHUD::OnUpdateMyAmmo_Implementation(int Ammo)
{
	//블루프린트에서 호출!
}

void AShootingGameHUD::OnUpdateMyMag_Implementation(int Mag)
{
	//블루프린트에서 호출!
}

void AShootingGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HudWidgetClass)
	{
		//블루프린트 변수에 UMG를 연결
		HudWidget = CreateWidget<UUserWidget>(GetWorld(), HudWidgetClass);
		//UI에 UMG 전시
		HudWidget->AddToViewport();
		//이벤트디스패처 바인딩
		BindPlayerState();
	}
}

void AShootingGameHUD::BindPlayerState()
{
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	/*UGameplayStatics::GetPlayerController(GetWorld(), 0);*/

	if (IsValid(pc) && IsValid(pc->PlayerState))
	{
		AShootingPlayerState* ps = Cast<AShootingPlayerState>(pc->PlayerState);
		if (IsValid(ps))
		{
			//ShootingPlayerState::Fuc_Dele_UpdateHp_TwoParams 이벤트디스패처 메시지에 대한 처리함수 및 파라미터를 지정한다. 
			ps->Fuc_Dele_UpdateHp_TwoParams.AddUFunction(this, FName("OnUpdateMyHp"));
			OnUpdateMyHp(ps->GetCurHp(), ps->GetMaxHp());

			//ShootingPlayerState::Fuc_Dele_UpdateMag_OneParam 이벤트디스패처 메시지에 대한 처리함수 및 파라미터를 지정한다. 
			ps->Fuc_Dele_UpdateMag_OneParam.AddUFunction(this, FName("OnUpdateMyMag"));
			OnUpdateMyMag(ps->GetMag());
			return;
		}
	}
	//PlayerCharacter 생성 후 PlayerState가 로드될때까지 기다린다.
	//Actor(PlayerController, PlayerCharacter)의 생성 시기를 정확하게 알수 없기 때문에 0.1초마다 재귀를 돌면서 기다린다...
	FTimerManager& timerManager = GetWorld()->GetTimerManager();
	timerManager.SetTimer(th_BindPlayerState, this, &AShootingGameHUD::BindPlayerState, 0.1f, false);
}
