#pragma once


#include "CoreMinimal.h"
#include "WeaponInterface.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Weapon.generated.h"

//블루프린트 사용 가능 구조체/블루프린트 데이터테이블을 사용하기 위해 FTableRowBase 상속
USTRUCT(BlueprintType)
struct FST_Weapon : public FTableRowBase
{
    GENERATED_BODY()
public:
    FST_Weapon() :
        StaticMesh(nullptr),
        ShootMontage(nullptr),
        ReloadMontage(nullptr),
        SoundBase(nullptr),
        FireEffect(nullptr),
        MaxAmmo(30),
        Damage(10)
    {}
    //총의 메쉬
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMesh* StaticMesh;
    //총 발사 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* ShootMontage;
    //총 장전 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* ReloadMontage;
    //발사음 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USoundBase* SoundBase;
    //발사효과(파티클시스템)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UParticleSystem* FireEffect;
    //장탄수
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int MaxAmmo;
    //대미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage;
};

//IWeaponInterface 상속!
UCLASS()
class SHOOTINGGAME_API AWeapon : public AActor, public IWeaponInterface
{
    GENERATED_BODY()
public:
    //
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMeshComponent* Mesh;
    //
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAudioComponent* Audio;
    //
    UPROPERTY(Replicated, BlueprintReadWrite, Meta = (ExposeOnSpawn = "true"))
    ACharacter* OwnChar;
    //
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = "true"))
    FName RowName;
    //
    FST_Weapon* WeaponData;
    //
    UPROPERTY(ReplicatedUsing = OnRep_Ammo)
    int Ammo;
    //
    float Damage;
public:
    AWeapon();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    //Replicated로 선언한 변수를 집어넣는다!
    void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;
    //--FIRE--------------------------------------------------------
    //Anim Montage Play!
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void PressTrigger(bool bIsPressed);
    virtual void PressTrigger_Implementation(bool bIsPressed) override;
    //Effect & Call Server LineTrace!
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void NotifyShoot();
    virtual void NotifyShoot_Implementation() override;
    //Check Ammo is Empty
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void IsCanUse(bool& bCanUse);
    virtual void IsCanUse_Implementation(bool& bCanUse) override;
    //Server LineTrace!
    UFUNCTION(Server, Reliable)
    void ReqShoot(const FVector vStart, const FVector vEnd);
    //Ammo : OnReplicateNotify
    UFUNCTION()
    void OnRep_Ammo();
    void UpdateAmmoUI(int Ammo);
    void ShowShootingEffect();
    bool UseAmmo();
    //--RELOAD------------------------------------------------------
    //Anim Montage Play!
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void PressReload();
    virtual void PressReload_Implementation() override;
    //Reload!
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void NotifyReload();
    virtual void NotifyReload_Implementation() override;
    UFUNCTION(BlueprintCallable)
    void DoReload();
    //--GRAB--------------------------------------------------------
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void AttachWeapon(ACharacter* Character);
    virtual void AttachWeapon_Implementation(ACharacter* Character) override;
    //--DROP--------------------------------------------------------
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void DetachWeapon(ACharacter* Character);
    virtual void DetachWeapon_Implementation(ACharacter* Character) override;
};
