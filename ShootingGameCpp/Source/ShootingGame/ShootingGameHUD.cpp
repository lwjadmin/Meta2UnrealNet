#include "ShootingGameHUD.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "ShootingPlayerState.h"

void AShootingGameHUD::OnUpdateMyHp_Implementation(float CurrentHp, float MaxHp)
{
	//�������Ʈ���� ȣ��!
}

void AShootingGameHUD::OnUpdateMyAmmo_Implementation(int Ammo)
{
	//�������Ʈ���� ȣ��!
}

void AShootingGameHUD::OnUpdateMyMag_Implementation(int Mag)
{
	//�������Ʈ���� ȣ��!
}

void AShootingGameHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HudWidgetClass)
	{
		//�������Ʈ ������ UMG�� ����
		HudWidget = CreateWidget<UUserWidget>(GetWorld(), HudWidgetClass);
		//UI�� UMG ����
		HudWidget->AddToViewport();
		//�̺�Ʈ����ó ���ε�
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
			//ShootingPlayerState::Fuc_Dele_UpdateHp_TwoParams �̺�Ʈ����ó �޽����� ���� ó���Լ� �� �Ķ���͸� �����Ѵ�. 
			ps->Fuc_Dele_UpdateHp_TwoParams.AddUFunction(this, FName("OnUpdateMyHp"));
			OnUpdateMyHp(ps->GetCurHp(), ps->GetMaxHp());

			//ShootingPlayerState::Fuc_Dele_UpdateMag_OneParam �̺�Ʈ����ó �޽����� ���� ó���Լ� �� �Ķ���͸� �����Ѵ�. 
			ps->Fuc_Dele_UpdateMag_OneParam.AddUFunction(this, FName("OnUpdateMyMag"));
			OnUpdateMyMag(ps->GetMag());
			return;
		}
	}
	//PlayerCharacter ���� �� PlayerState�� �ε�ɶ����� ��ٸ���.
	//Actor(PlayerController, PlayerCharacter)�� ���� �ñ⸦ ��Ȯ�ϰ� �˼� ���� ������ 0.1�ʸ��� ��͸� ���鼭 ��ٸ���...
	FTimerManager& timerManager = GetWorld()->GetTimerManager();
	timerManager.SetTimer(th_BindPlayerState, this, &AShootingGameHUD::BindPlayerState, 0.1f, false);
}
