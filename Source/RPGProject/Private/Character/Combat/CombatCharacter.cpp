// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Combat/CombatCharacter.h"


#include "Components/Status/StatsComponent.h"
#include "GameFramework/DamageType.h"
#include "HitBox/HitBoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Shared/Action/Components/ActionComponent.h"

ACombatCharacter::ACombatCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    HitBoxComponent = CreateDefaultSubobject<UHitBoxComponent>(TEXT("HitBoxComponent"));
}

void ACombatCharacter::BeginPlay()
{
    Super::BeginPlay();

    // HitBox 이벤트 바인딩
    if (HitBoxComponent)
    {
        // HitBoxComponent->OnHitBoxHit.AddDynamic(this, &ACombatCharacter::OnHitBoxHit);
    }
}

void ACombatCharacter::BeginDestroy()
{
    // 델리게이트 정리
    OnCharacterParried.Clear();
    OnCombatStateChanged.Clear();
    OnWeaponEquipped.Clear();
    OnWeaponUnequipped.Clear();

    // 타이머 정리
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(ParryWindowTimer);
        GetWorld()->GetTimerManager().ClearTimer(CombatStateTimer);
    }

    Super::BeginDestroy();
}

#pragma region Combat_Interface

bool ACombatCharacter::CanAttack() const
{
    if (IsDead())
    {
        return false;
    }


    return !bIsAttacking;
}

bool ACombatCharacter::CanBlock() const
{
    if (IsDead())
    {
        return false;
    }
    

    return !bIsAttacking;
}

bool ACombatCharacter::CanDodge() const
{
    // if (IsDead() || IsInvincible())
    // {
    //     return false;
    // }
    

    return true;
}

bool ACombatCharacter::CanParry() const
{
    return bCanParry && CanBlock();
}

bool ACombatCharacter::IsAttacking() const
{
    return bIsAttacking;
}

bool ACombatCharacter::IsBlocking() const
{
    return bIsBlocking;
}


#pragma endregion

#pragma region Combat_Actions

void ACombatCharacter::StartAttack()
{
    if (!CanAttack())
    {
        return;
    }

    bIsAttacking = true;
    

    if (ActionComponent)
    {
        // ActionComponent->StartActionByTag(this, FGameplayTag::RequestGameplayTag(FName("Action.Attack.Basic")));
    }
}

void ACombatCharacter::EndAttack()
{
    bIsAttacking = false;
    

}

void ACombatCharacter::StartBlock()
{
    if (!CanBlock())
    {
        return;
    }

    bIsBlocking = true;

    // 패리 윈도우 시작
    OnParryWindowStart();
}

void ACombatCharacter::EndBlock()
{
    bIsBlocking = false;
    bCanParry = false;

    // 패리 윈도우 종료
    OnParryWindowEnd();
    
}

bool ACombatCharacter::TryParry()
{
    if (!CanParry())
    {
        return false;
    }

    // 패리 성공 - 델리게이트 브로드캐스트는 데미지 계산에서 처리
    return true;
}

void ACombatCharacter::Dodge(const FVector& Direction)
{
    if (!CanDodge())
    {
        return;
    }


    // Action 시스템을 통한 회피 실행
    if (ActionComponent)
    {
        // ActionComponent->StartActionByTag(this, FGameplayTag::RequestGameplayTag(FName("Action.Dodge")));
    }
}

#pragma endregion

#pragma region Damage_System

float ACombatCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
    class AController* EventInstigator, AActor* DamageCauser)
{
    // if (IsDead() || IsInvincible())
    // {
    //     return 0.0f;
    // }

    float ActualDamage = DamageAmount;

    // 방어 처리
    if (bIsBlocking && DamageCauser)
    {
        FVector AttackDirection = (GetActorLocation() - DamageCauser->GetActorLocation()).GetSafeNormal();
        
        if (IsAngleWithinBlockRange(AttackDirection))
        {
            // 패리 체크
            if (bCanParry)
            {
                // 패리 성공
                OnCharacterParried.Broadcast(DamageCauser);
                OnSuccessfulParry(DamageCauser);
                return 0.0f;
            }
            else
            {
                // 일반 방어
                ActualDamage *= 0.3f; // 70% 데미지 감소
                OnDamageBlocked(ActualDamage, DamageCauser);
            }
        }
    }
    

    // 스탯 컴포넌트를 통한 데미지 적용
    if (StatsComponent)
    {
        FGameplayTag HealthTag = FGameplayTag::RequestGameplayTag(FName("Character.Stat.Health"));
        // StatsComponent->RemoveStatValue(HealthTag, ActualDamage);

        // 체력이 0 이하가 되면 사망
        // if (StatsComponent->GetStatValue(HealthTag) <= 0.0f)
        // {
        //     Die();
        // }
    }

    // 히트 이펙트
    if (DamageCauser)
    {
        FVector HitLocation = GetActorLocation();
        FVector HitNormal = (GetActorLocation() - DamageCauser->GetActorLocation()).GetSafeNormal();
        // SpawnHitEffect(HitLocation, HitNormal);
        // PlayHitSound();
    }

    // 히트스톱 적용
    // ApplyHitStop(0.1f);

    // 델리게이트 브로드캐스트
    OnDamageReceived(ActualDamage, DamageCauser);

    return Super::TakeDamage(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
}

#pragma endregion


#pragma region Weapon_System

// bool ACombatCharacter::IsWeaponEquipped() const
// {
//     if (CombatComponent)
//     {
//         return CombatComponent->IsWeaponEquipped();
//     }
//     return false;
// }
//
// FGameplayTag ACombatCharacter::GetEquippedWeaponType() const
// {
//     if (CombatComponent)
//     {
//         return CombatComponent->GetEquippedWeaponTypeTag();
//     }
//     return FGameplayTag();
// }
//
// void ACombatCharacter::EquipWeapon(FGameplayTag WeaponType)
// {
//     if (CombatComponent)
//     {
//         CombatComponent->EquipWeapon(WeaponType);
//         OnWeaponEquipped.Broadcast(WeaponType);
//     }
// }
//
// void ACombatCharacter::UnequipWeapon()
// {
//     if (CombatComponent)
//     {
//         CombatComponent->UnequipWeapon();
//         OnWeaponUnequipped.Broadcast();
//     }
// }

#pragma endregion

#pragma region Combat_Stats

void ACombatCharacter::ModifyAttackDamage(float Multiplier)
{
    BaseAttackDamage *= Multiplier;
}

void ACombatCharacter::ModifyAttackRange(float Multiplier)
{
    BaseAttackRange *= Multiplier;
}

#pragma endregion

void ACombatCharacter::OnParryWindowStart()
{
    bCanParry = true;

    // 패리 윈도우 타이머 설정
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            ParryWindowTimer,
            this,
            &ACombatCharacter::OnParryWindowEnd,
            ParryWindow,
            false
        );
    }
}

void ACombatCharacter::OnParryWindowEnd()
{
    bCanParry = false;
    
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(ParryWindowTimer);
    }
}

bool ACombatCharacter::IsAngleWithinBlockRange(const FVector& AttackDirection) const
{
    FVector ForwardVector = GetActorForwardVector();
    float DotProduct = FVector::DotProduct(ForwardVector, AttackDirection);
    float AngleInDegrees = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
    
    return AngleInDegrees <= (BlockAngle / 2.0f);
}

UFUNCTION()
void ACombatCharacter::OnHitBoxHit(const FHitBoxDamageEvent& HitEvent)
{
    // HitBox에서 발생한 히트 이벤트 처리
    if (HitEvent.HitActor)
    {
        // 데미지 적용
        UGameplayStatics::ApplyDamage(
            HitEvent.HitActor,
            HitEvent.FinalDamage,
            GetInstigatorController(),
            this,
            UDamageType::StaticClass()
        );

        // 이펙트 재생
        // SpawnHitEffect(HitEvent.HitLocation, HitEvent.HitNormal);
        // PlayHitSound();
    }
}