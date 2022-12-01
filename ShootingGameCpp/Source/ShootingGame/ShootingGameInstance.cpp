// Fill out your copyright notice in the Description page of Project Settings.



#include "ShootingGameInstance.h"


FST_Weapon* UShootingGameInstance::GetWeaponRowData(FName name)
{
	return  WeaponTable->FindRow<FST_Weapon>(name, TEXT(""));
}


FName UShootingGameInstance::GetWeaponRandomRowName()
{
	TArray<FName> WeaponNameList = WeaponTable->GetRowNames();
	FName RowName = WeaponNameList[FMath::RandRange(0, WeaponNameList.Num() - 1)];
	return RowName;
}
