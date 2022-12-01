
#include "Weapon.h"
#include "ShootingGameHUD.h"
#include "ShootingPlayerState.h"
#include "GameFramework/Character.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DataTable.h"
#include "ShootingGameInstance.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));

	RootComponent = Mesh;
	Audio->SetupAttachment(RootComponent);

	//Custom Collision Profile : Weapon ����
	Mesh->SetCollisionProfileName("Weapon");
	//�ʱ���� : SetSimulatePhysics = true�� üũ�Ͽ� ���ٴڿ� ���������� ����
	Mesh->SetSimulatePhysics(true);
	//Weapon Actor ��ü�� Replicate ó���Ͽ� ����/Ŭ���̾�Ʈ���� ��� ���̵��� true üũ
	bReplicates = true;
	//Weapon Actor ��ġ�� �������� ��� ����/Ŭ�� ��� �����ϰ� �����̱� ���� true üũ
	SetReplicateMovement(true);
	Ammo = 0;
	Damage = 0;
}

void AWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, OwnChar);
	DOREPLIFETIME(AWeapon, RowName);
	DOREPLIFETIME(AWeapon, Ammo);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::PressTrigger_Implementation(bool IsPressed)
{
	//�÷��̾ �߻� ��ư ������ �ִϸ��̼� ��Ÿ�� ����
	//AnimBP -> NotifyShoot ȣ��
	if (IsPressed)
	{
		OwnChar->PlayAnimMontage(WeaponData->ShootMontage);
	}
}

void AWeapon::PressReload_Implementation()
{
	AShootingPlayerState* ps = Cast<AShootingPlayerState>(OwnChar->GetPlayerState());
	if (ps && ps->IsCanUseMag())
	{
		OwnChar->PlayAnimMontage(WeaponData->ReloadMontage);
	}
}

void AWeapon::NotifyReload_Implementation()
{
	DoReload();
}

void AWeapon::DoReload()
{
	this->Ammo = WeaponData->MaxAmmo;
	OnRep_Ammo();
	AShootingPlayerState* ps = Cast<AShootingPlayerState>(OwnChar->GetPlayerState());
	if (ps)
	{
		ps->UseMag();
	}
}

void AWeapon::IsCanUse_Implementation(bool& bCanUse)
{
	bCanUse = Ammo > 0 ? true : false;
}

void AWeapon::OnRep_Ammo()
{
	UpdateAmmoUI(this->Ammo);
}

void AWeapon::OnRep_RowName()
{
	UShootingGameInstance* gameInstance = Cast< UShootingGameInstance>(GetGameInstance());
	if (gameInstance)
	{
		WeaponData = gameInstance->GetWeaponRowData(RowName);
		Mesh->SetStaticMesh(WeaponData->StaticMesh);
		Audio->SetSound(WeaponData->SoundBase);
		Ammo = WeaponData->MaxAmmo;
		Damage = WeaponData->Damage;
	}
}

void AWeapon::UpdateAmmoUI(int InAmmo)
{
	APlayerController* firstplayer = GetWorld()->GetFirstPlayerController();

	if (IsValid(OwnChar) && OwnChar->GetController() == firstplayer)
	{
		AShootingGameHUD* hud = Cast<AShootingGameHUD>(firstplayer->GetHUD());
		if (IsValid(hud))
		{
			hud->OnUpdateMyAmmo(InAmmo);
		}
	}
}

void AWeapon::ShowShootingEffect()
{
	//Weapon StaticMesh�� Muzzle �����߰�, Muzzle��ġ�� Emitter �߻� 
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponData->FireEffect, Mesh->GetSocketLocation("Muzzle"), Mesh->GetSocketRotation("Muzzle"), FVector(0.3f, 0.3f, 0.3f));
	Audio->Play();
}

bool AWeapon::UseAmmo()
{
	if (Ammo <= 0)
	{
		return false;
	}
	Ammo = Ammo - 1;
	OnRep_Ammo();
	return true;
}

void AWeapon::SetRowName(FName name)
{ 
	RowName = name;
	OnRep_RowName();
}

void AWeapon::NotifyShoot_Implementation()
{
	//���⿡ �Ѿ��� �������� ���
	bool bval = false;
	IsCanUse(bval);
	if (bval)
	{
		//����Ʈ �߻�
		ShowShootingEffect();
		APlayerController* shooter = GetWorld()->GetFirstPlayerController();
		if (shooter == OwnChar->GetController())
		{
			//ī�޶� ��ġ + ī�޶���� 350���� ī�޶���� 5000���� Ray �߻� (����)
			//Predict Projectile Path By TraceChannel : � Ray �߻� 
			FVector forward = shooter->PlayerCameraManager->GetActorForwardVector();
			FVector start = (forward * 350) + shooter->PlayerCameraManager->GetCameraLocation();
			FVector end = (forward * 5000) + shooter->PlayerCameraManager->GetCameraLocation();
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Client - ReqShoot")));
			//�������� Rayó��
			ReqShoot(start, end);
		}
	}
}

void AWeapon::ReqShoot_Implementation(const FVector vStart, const FVector vEnd)
{
	//�������� Rayó��
	//Ammo ���� 
	if (UseAmmo())
	{
		FHitResult result;
		bool isHit = GetWorld()->LineTraceSingleByObjectType(result, vStart, vEnd, ECollisionChannel::ECC_Pawn);
		DrawDebugLine(GetWorld(), vStart, vEnd, FColor::Yellow, false, 5.0f);
		if (isHit)
		{
			ACharacter* HitChar = Cast<ACharacter>(result.GetActor());
			if (HitChar)
			{
				//AddDamage
				UGameplayStatics::ApplyDamage(HitChar, 10, OwnChar->GetController(), this, UDamageType::StaticClass());
			}
		}
	}
}

void AWeapon::AttachWeapon_Implementation(ACharacter* Character)
{
	//�� �ݱ�
	OwnChar = Character;
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//ĳ���Ϳ� weapon ������ �޷��־�� ��!(������)
	this->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("weapon"));
	UpdateAmmoUI(Ammo);
}

void AWeapon::DetachWeapon_Implementation(ACharacter* Character)
{
	//�� ������
	UpdateAmmoUI(0);
	OwnChar = nullptr;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);
}

