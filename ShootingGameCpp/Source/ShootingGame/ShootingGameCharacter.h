#pragma once
#include "CoreMinimal.h"
#include "ItemInterface.h"
#include "GameFramework/Character.h"
#include "ShootingGameCharacter.generated.h"

UCLASS(config = Game)
class AShootingGameCharacter : public ACharacter, public IItemInterface
{
    GENERATED_BODY()
public:
    //
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    //
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
    //
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* EquipWeapon;
    FORCEINLINE class AActor* GetEquipWeapon() const { return EquipWeapon; }
    //
    UPROPERTY(Replicated)
    float ControlPitch;
    //
    bool IsRagdoll;
    //
    FTimerHandle th_BindPlayerState;
    //
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UUserWidget> NameTagWidgetClass;
    //
    UPROPERTY(BlueprintReadWrite)
    UUserWidget* NameTagWidget;
public:
    AShootingGameCharacter();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;
    //--BindPlayerState---------------------------------------------------------------------------------
    void BindPlayerState();
    UFUNCTION(BlueprintNativeEvent)
    void OnUpdateHp(float CurrentHp, float MaxHp); //Virtual, Blueprint에서 Override할 수 있다. 
    void OnUpdateHp_Implementation(float CurrentHp, float MaxHp);
    //--Moving------------------------------------------------------------------------------------------
    void MoveForward(float Value);
    void MoveRight(float Value);
    UFUNCTION(BlueprintPure)
    FORCEINLINE float GetControlPitch() const { return ControlPitch; }
    //--TakeDamage--------------------------------------------------------------------------------------
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
    UFUNCTION(BlueprintCallable)
    void DoRagdoll();
    UFUNCTION(BlueprintCallable)
    void DoGetup();
    UFUNCTION(Server, Reliable)
    void ReqPressRagdoll();
    UFUNCTION(NetMulticast, Reliable)
    void ResPressRagdoll();
    //--Pickup------------------------------------------------------------------------------------------
    void PressPickup();
    UFUNCTION(Server, Reliable)
    void ReqPickup();
    UFUNCTION(NetMulticast, Reliable)
    void ResPickup(AActor* weapon);
    AActor* GetNearestWeapon();
    void EnableOwnerWeapon(AActor* Weapon);
    void AttachWeapon(AActor* Weapon);
    //--Drop--------------------------------------------------------------------------------------------
    void PressDropWeapon();
    UFUNCTION(Server, Reliable)
    void ReqDropWeapon();
    UFUNCTION(NetMulticast, Reliable)
    void ResDropWeapon();
    void DisableOwnerWeapon();
    void DetachWeapon();
    //--Fire--------------------------------------------------------------------------------------------
    void PressTrigger();
    void ReleaseTrigger();
    UFUNCTION(Server, Reliable)
    void ReqPressTrigger(bool isPressed);
    UFUNCTION(NetMulticast, Reliable)
    void ResPressTrigger(bool isPressed);
    UFUNCTION(BlueprintCallable)
    void OnNotifyShoot();
    //--Reload------------------------------------------------------------------------------------------
    void PressReload();
    UFUNCTION(Server, Reliable)
    void ReqPressReload();
    UFUNCTION(NetMulticast, Reliable)
    void ResPressReload();
    UFUNCTION(BlueprintCallable)
    void OnNotifyReload();
    //--ItemPlugin_ItemInterface_AddHeal----------------------------------------------------------------
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void AddHeal(float HealVal);
    virtual void AddHeal_Implementation(float HealVal) override; 
    //--GetMagazine-------------------------------------------------------------------------------------
    void PressMagazine();
};

