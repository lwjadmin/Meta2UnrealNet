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
	//bIsPressed = true : 버튼을 눌렀을 때
	//bIsPressed = false : 버튼을 뗐을 때
	if (bIsPressed)
	{
		//Fire버튼 눌렀을 때 0.2초 후 BurstShooting 호출
		FTimerManager& timerManager = GetWorld()->GetTimerManager();
		timerManager.SetTimer(th_BurstShooting, this, &AWeaponBurst::BurstShooting, 0.2f, true);
		//초탄 딜레이 (0.2초)를 없애기 위해 바로 BurstShooting 호출
		BurstShooting();
	}
	else
	{
		//Fire버튼 뗐을 때 BurstShooting 타이머 종료
		FTimerManager& timerManager = GetWorld()->GetTimerManager();
		timerManager.ClearTimer(th_BurstShooting);
	}
}

void AWeaponBurst::BurstShooting()
{
	if (OwnChar == nullptr)
	{
		//Fire버튼 뗐을 때 BurstShooting 타이머 종료
		FTimerManager& timerManager = GetWorld()->GetTimerManager();
		timerManager.ClearTimer(th_BurstShooting);
		return;
	}
	//AnimMontage 실행, Animation Blueprint : AnimNotify에서 Weapon::NotifyShoot 함수 호출
	OwnChar->PlayAnimMontage(WeaponData->ShootMontage);
}
	
