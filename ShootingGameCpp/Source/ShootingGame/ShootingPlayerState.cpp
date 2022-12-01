// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootingPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "ShootingGameHUD.h"

void AShootingPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShootingPlayerState, CurHp);
	DOREPLIFETIME(AShootingPlayerState, MaxHp);
	DOREPLIFETIME(AShootingPlayerState, Mag);
}

AShootingPlayerState::AShootingPlayerState()
{
	CurHp = 100.0f;
	MaxHp = 100.0f;
	Mag = 0;
}

void AShootingPlayerState::OnRep_CurHp()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("OnRep_CurHp = %f"), CurHp));

	if(Fuc_Dele_UpdateHp_TwoParams.IsBound())
		Fuc_Dele_UpdateHp_TwoParams.Broadcast(CurHp, MaxHp);
}

void AShootingPlayerState::OnRep_MaxHp()
{
}

void AShootingPlayerState::OnRep_Mag()
{
	if (Fuc_Dele_UpdateMag_OneParam.IsBound())
		Fuc_Dele_UpdateMag_OneParam.Broadcast(Mag);
}

void AShootingPlayerState::AddDamage(float Damage)
{
	CurHp = CurHp - Damage;
	CurHp = FMath::Clamp<float>(CurHp, 0.0f, MaxHp);
	OnRep_CurHp();
}

void AShootingPlayerState::AddHeal(float HealVal)
{
	CurHp = CurHp + HealVal;
	CurHp = FMath::Clamp<float>(CurHp, 0.0f, MaxHp);
	OnRep_CurHp();
}

void AShootingPlayerState::AddMag()
{
	Mag = Mag + 1;
	OnRep_Mag();
}
