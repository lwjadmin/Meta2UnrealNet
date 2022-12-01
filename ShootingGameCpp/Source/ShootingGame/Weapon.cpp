
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

	//Custom Collision Profile : Weapon 설정
	Mesh->SetCollisionProfileName("Weapon");
	//초기상태 : SetSimulatePhysics = true로 체크하여 땅바닥에 떨어지도록 설정
	Mesh->SetSimulatePhysics(true);
	//Weapon Actor 자체를 Replicate 처리하여 서버/클라이언트에서 모두 보이도록 true 체크
	bReplicates = true;
	//Weapon Actor 위치가 움직였을 경우 서버/클라 모두 동일하게 움직이기 위해 true 체크
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
	//플레이어가 발사 버튼 누르면 애니메이션 몽타주 실행
	//AnimBP -> NotifyShoot 호출
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
	//Weapon StaticMesh에 Muzzle 소켓추가, Muzzle위치에 Emitter 발생 
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
	//무기에 총알이 남아있을 경우
	bool bval = false;
	IsCanUse(bval);
	if (bval)
	{
		//이펙트 발생
		ShowShootingEffect();
		APlayerController* shooter = GetWorld()->GetFirstPlayerController();
		if (shooter == OwnChar->GetController())
		{
			//카메라 위치 + 카메라방향 350에서 카메라방향 5000으로 Ray 발사 (직선)
			//Predict Projectile Path By TraceChannel : 곡선 Ray 발사 
			FVector forward = shooter->PlayerCameraManager->GetActorForwardVector();
			FVector start = (forward * 350) + shooter->PlayerCameraManager->GetCameraLocation();
			FVector end = (forward * 5000) + shooter->PlayerCameraManager->GetCameraLocation();
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Client - ReqShoot")));
			//서버에서 Ray처리
			ReqShoot(start, end);
		}
	}
}

void AWeapon::ReqShoot_Implementation(const FVector vStart, const FVector vEnd)
{
	//서버에서 Ray처리
	//Ammo 감소 
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
	//총 줍기
	OwnChar = Character;
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//캐릭터에 weapon 소켓이 달려있어야 함!(오른손)
	this->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("weapon"));
	UpdateAmmoUI(Ammo);
}

void AWeapon::DetachWeapon_Implementation(ACharacter* Character)
{
	//총 버리기
	UpdateAmmoUI(0);
	OwnChar = nullptr;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);
}

