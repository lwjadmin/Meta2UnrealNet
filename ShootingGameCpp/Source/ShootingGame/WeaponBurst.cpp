// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBurst.h"
#include "ShootingGameHUD.h"
#include "Weapon.h"
#include "GameFramework/Character.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DataTable.h"
#include "ShootingGameInstance.h"

void AWeaponBurst::PressTrigger_Implementation(bool bIsPressed)
{
	//bIsPressed = true : ��ư�� ������ ��
	//bIsPressed = false : ��ư�� ���� ��
	if (bIsPressed)
	{
		//Fire��ư ������ �� 0.2�� �� BurstShooting ȣ��
		FTimerManager& timerManager = GetWorld()->GetTimerManager();
		timerManager.SetTimer(th_BurstShooting, this, &AWeaponBurst::BurstShooting, 0.2f, true);
		//��ź ������ (0.2��)�� ���ֱ� ���� �ٷ� BurstShooting ȣ��
		BurstShooting();
	}
	else
	{
		//Fire��ư ���� �� BurstShooting Ÿ�̸� ����
		FTimerManager& timerManager = GetWorld()->GetTimerManager();
		timerManager.ClearTimer(th_BurstShooting);
	}
}

void AWeaponBurst::BurstShooting()
{
	if (OwnChar == nullptr)
	{
		//Fire��ư ���� �� BurstShooting Ÿ�̸� ����
		FTimerManager& timerManager = GetWorld()->GetTimerManager();
		timerManager.ClearTimer(th_BurstShooting);
		return;
	}
	//AnimMontage ����, Animation Blueprint : AnimNotify���� Weapon::NotifyShoot �Լ� ȣ��
	OwnChar->PlayAnimMontage(WeaponData->ShootMontage);
}
	
