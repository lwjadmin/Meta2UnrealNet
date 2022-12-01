#pragma once


#include "CoreMinimal.h"
#include "WeaponInterface.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Weapon.generated.h"

//�������Ʈ ��� ���� ����ü/�������Ʈ ���������̺��� ����ϱ� ���� FTableRowBase ���
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
    //���� �޽�
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMesh* StaticMesh;
    //�� �߻� �ִϸ��̼� ��Ÿ��
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* ShootMontage;
    //�� ���� �ִϸ��̼� ��Ÿ��
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimMontage* ReloadMontage;
    //�߻��� 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USoundBase* SoundBase;
    //�߻�ȿ��(��ƼŬ�ý���)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UParticleSystem* FireEffect;
    //��ź��
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int MaxAmmo;
    //�����
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage;
};

//IWeaponInterface ���!
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
    //Replicated�� ������ ������ ����ִ´�!
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
