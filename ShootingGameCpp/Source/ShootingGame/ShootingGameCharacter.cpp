//Custom Header
#include "ShootingGameCharacter.h"
#include "WeaponInterface.h"
#include "ShootingPlayerState.h"
#include "Weapon.h"
#include "NameTagInterface.h"
//Unreal Header
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

AShootingGameCharacter::AShootingGameCharacter()
{
    //default Creation
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.2f;
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f;
    CameraBoom->bUsePawnControlRotation = true;
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
    //랙돌 = false 설정!
    IsRagdoll = false;
    //3rdPersonView로 설정!
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bUseControllerDesiredRotation = true;
    GetCharacterMovement()->bOrientRotationToMovement = true;

}

void AShootingGameCharacter::BeginPlay()
{
    Super::BeginPlay();
    BindPlayerState();
}

void AShootingGameCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    //Server일 경우, Pitch값을 셋팅해주자. (Client Multicast용)
    if (HasAuthority() == true)
    {
        ControlPitch = GetControlRotation().Pitch;
    }
    //피가 0이 되어 랙돌이 되었을 때, 플레이어가 움직일 수 없도록 플레이어 메쉬 허리기준 Z축+60cm 상으로 위치를 고정한다.
    if (IsRagdoll)
    {
        SetActorLocation(GetMesh()->GetSocketLocation(FName(TEXT("spine_01"))) + FVector(0, 0, 60));
    }
}

void AShootingGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    check(PlayerInputComponent);
    //Action Mapping
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
    PlayerInputComponent->BindAction("Trigger", IE_Pressed, this, &AShootingGameCharacter::PressTrigger);
    PlayerInputComponent->BindAction("Trigger", IE_Released, this, &AShootingGameCharacter::ReleaseTrigger);

    PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AShootingGameCharacter::PressReload);
    PlayerInputComponent->BindAction("Pickup", IE_Pressed, this, &AShootingGameCharacter::PressPickup);
    PlayerInputComponent->BindAction("DropWeapon", IE_Pressed, this, &AShootingGameCharacter::PressDropWeapon);
    PlayerInputComponent->BindAction("MagTest", IE_Pressed, this, &AShootingGameCharacter::PressMagazine);
    //Axis Mapping
    PlayerInputComponent->BindAxis("MoveForward", this, &AShootingGameCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AShootingGameCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void AShootingGameCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    //Replicated Value Mapping
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AShootingGameCharacter, ControlPitch);
}

void AShootingGameCharacter::BindPlayerState()
{
    //시작시 재귀 타이머를 돌며 PlayerState가 로드될 때까지 기다리고, 로드되면 UI에 HP를 Update한다. 
    AShootingPlayerState* ps = Cast<AShootingPlayerState>(GetPlayerState());
    if (IsValid(ps))
    {
        ps->Fuc_Dele_UpdateHp_TwoParams.AddUFunction(this, FName("OnUpdateHp"));
        OnUpdateHp(ps->GetCurHp(), ps->GetMaxHp());
        return;
    }
    FTimerManager& timerManager = GetWorld()->GetTimerManager();
    timerManager.SetTimer(th_BindPlayerState, this, &AShootingGameCharacter::BindPlayerState, 0.1f, false);
}

void AShootingGameCharacter::OnUpdateHp_Implementation(float CurrentHp, float MaxHp)
{
    if (CurrentHp <= 0)
    {
        DoRagdoll();
    }
}

void AShootingGameCharacter::MoveForward(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AShootingGameCharacter::MoveRight(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        // find out which way is right
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get right vector 
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        // add movement in that direction
        AddMovementInput(Direction, Value);
    }
}

float AShootingGameCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("TakeDamage Damage=%f EventInstigator=%s"), DamageAmount, *EventInstigator->GetName()));
    
    //대미지를 받을 경우, PlayerState에 피격처리함수를 호출한다.
    AShootingPlayerState* ps = Cast<AShootingPlayerState>(GetPlayerState());
    if (ps)
    {
        ps->AddDamage(DamageAmount);
    }
    return 0.0f;
}

void AShootingGameCharacter::ReqPressRagdoll_Implementation()
{
    ResPressRagdoll();
}
void AShootingGameCharacter::ResPressRagdoll_Implementation()
{
    if (IsRagdoll)
    {
        DoGetup();
    }
    else
    {
        DoRagdoll();
    }
}
void AShootingGameCharacter::DoRagdoll()
{
    IsRagdoll = true;
    GetMesh()->SetSimulatePhysics(IsRagdoll);
}
void AShootingGameCharacter::DoGetup()
{
    IsRagdoll = false;
    GetMesh()->SetSimulatePhysics(IsRagdoll);
    GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
    FVector location = { 0.0f,0.0f,-97.0f };
    FRotator rotation = { 0.0f,270.0f,0.0f };
    GetMesh()->SetRelativeLocationAndRotation(location, rotation);
}
#pragma region --PICKUP-------------------------------------------------------------
void AShootingGameCharacter::PressPickup()
{
    ReqPickup();
}
void AShootingGameCharacter::ReqPickup_Implementation() //Run On Server 
{
    //주변의 총을 찾는다.
    AActor* weapon = GetNearestWeapon();
    if (weapon)
    {
        //[서버]현재 총을 쥐고 있다면 놓는다.
        if (IsValid(EquipWeapon))
        {
            IWeaponInterface* InterfaceObj = Cast<IWeaponInterface>(EquipWeapon);
            if (InterfaceObj)
            {
                InterfaceObj->Execute_DetachWeapon(EquipWeapon, this);
            }
        }
        //[서버전용]Weapon의 Owner를 설정한다.
        EnableOwnerWeapon(weapon);
        ResPickup(weapon);
    }
}
void AShootingGameCharacter::ResPickup_Implementation(AActor* weapon) //Multicast
{
    if (!IsValid(weapon)) { return; }
    if (IsValid(EquipWeapon))
    {
        //[클라]현재 총을 쥐고 있다면 놓는다.
        IWeaponInterface* InterfaceObj = Cast<IWeaponInterface>(EquipWeapon);
        if (InterfaceObj)
        {
            InterfaceObj->Execute_DetachWeapon(EquipWeapon, this);
        }
    }
    AttachWeapon(weapon);
}
void AShootingGameCharacter::EnableOwnerWeapon(AActor* Weapon)
{
    Weapon->SetOwner(this->GetController());
}
void AShootingGameCharacter::AttachWeapon(AActor* weapon)
{
    if (!IsValid(weapon)) { return; }
    EquipWeapon = weapon;
    IWeaponInterface* InterfaceObj = Cast<IWeaponInterface>(EquipWeapon);
    if (InterfaceObj)
    {
        InterfaceObj->Execute_AttachWeapon(EquipWeapon, this);
        //FPS시점으로 전환
        bUseControllerRotationYaw = true;
        GetCharacterMovement()->bUseControllerDesiredRotation = false;
        GetCharacterMovement()->bOrientRotationToMovement = false;
    }
}
AActor* AShootingGameCharacter::GetNearestWeapon()
{
    TArray<AActor*> CollectedActors;
    AActor* NearestActor = nullptr;
    float NearDistance = 100000.0f;

    GetCapsuleComponent()->GetOverlappingActors(CollectedActors, AWeapon::StaticClass());
    for (AActor* weapon : CollectedActors)
    {
        IWeaponInterface* interface = Cast<IWeaponInterface>(weapon);
        if (interface == nullptr) { continue; }
        float dist = FVector::Distance(GetActorLocation(), weapon->GetActorLocation());
        if (NearDistance > dist)
        {
            NearDistance = dist;
            NearestActor = weapon;
        }
    }
    return NearestActor;
}
#pragma endregion
#pragma region --DROP--------------------------------------------------------
void AShootingGameCharacter::PressDropWeapon()
{
    ReqDropWeapon();
}

void AShootingGameCharacter::ReqDropWeapon_Implementation()
{
    //[서버]EquipWeapon Owner를 Null로 설정한다.
    DisableOwnerWeapon();
    ResDropWeapon();
}

void AShootingGameCharacter::ResDropWeapon_Implementation()
{
    DetachWeapon();
}
void AShootingGameCharacter::DisableOwnerWeapon()
{
    if (IsValid(EquipWeapon))
    {
        EquipWeapon->SetOwner(nullptr);
    }
}
void AShootingGameCharacter::DetachWeapon()
{
    IWeaponInterface* InterfaceObj = Cast<IWeaponInterface>(EquipWeapon);
    if (InterfaceObj)
    {
        InterfaceObj->Execute_DetachWeapon(EquipWeapon, this);
        EquipWeapon = nullptr;
        //3rdPerson 시점전환
        bUseControllerRotationYaw = false;
        GetCharacterMovement()->bUseControllerDesiredRotation = true;
        GetCharacterMovement()->bOrientRotationToMovement = true;
    }
}

#pragma endregion
#pragma region --FIRE----------------------------------------------------------------
void AShootingGameCharacter::PressTrigger()
{
    ReqPressTrigger(true);
}

void AShootingGameCharacter::ReleaseTrigger()
{
    ReqPressTrigger(false);
}

void AShootingGameCharacter::ReqPressTrigger_Implementation(bool IsPressed) //Run On Server
{
    IWeaponInterface* InterfaceObj = Cast<IWeaponInterface>(EquipWeapon);
    if (InterfaceObj)
    {
        bool IsCanUse = false;
        InterfaceObj->Execute_IsCanUse(EquipWeapon, IsCanUse);
        //총알이 없으면 return
        if (IsCanUse == false)
        {
            return;
        }
    }
    ResPressTrigger(IsPressed);
}
void AShootingGameCharacter::ResPressTrigger_Implementation(bool IsPressed)
{
    //Execute_PressTrigger : Anim Montage 실행!
    IWeaponInterface* InterfaceObj = Cast<IWeaponInterface>(EquipWeapon);
    if (InterfaceObj)
    {
        InterfaceObj->Execute_PressTrigger(EquipWeapon, IsPressed);
    }
}

void AShootingGameCharacter::OnNotifyShoot()
{
    //AnimBP Anim Montage Notify -> OnNotifyShoot 호출
    IWeaponInterface* InterfaceObj = Cast<IWeaponInterface>(EquipWeapon);
    if (InterfaceObj)
    {
        //Ray쏘기, 피격처리
        InterfaceObj->Execute_NotifyShoot(EquipWeapon);
    }
}
#pragma endregion
#pragma region --RELOAD----------------------------------------------------------------
void AShootingGameCharacter::PressReload()
{
    ReqPressReload();
}
void AShootingGameCharacter::ReqPressReload_Implementation() //Run On Server
{
    ResPressReload();
}
void AShootingGameCharacter::ResPressReload_Implementation() //Multicast
{
    IWeaponInterface* InterfaceObj = Cast<IWeaponInterface>(EquipWeapon);
    if (InterfaceObj)
    {
        //Execute_PressReload : Anim Montage 실행!
        InterfaceObj->Execute_PressReload(EquipWeapon);
    }
}
void AShootingGameCharacter::OnNotifyReload()
{
    //AnimBP Anim Montage Notify -> NotifyReload 호출
    IWeaponInterface* InterfaceObj = Cast<IWeaponInterface>(EquipWeapon);
    if (InterfaceObj)
    {
        InterfaceObj->Execute_NotifyReload(EquipWeapon);
    }
}

#pragma endregion
void AShootingGameCharacter::AddHeal_Implementation(float HealVal)
{
    AShootingPlayerState* ps = Cast<AShootingPlayerState>(GetPlayerState());
    if (ps)
    {
        ps->AddHeal(HealVal);
    }
}
void AShootingGameCharacter::PressMagazine()
{
    AShootingPlayerState* ps = Cast<AShootingPlayerState>(GetPlayerState());
    if (ps)
    {
        ps->AddMag();
    }
}

