
#include "Character/BaseCharacter.h"

#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/Status/BuffComponent.h"
#include "Components/Status/StatsComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Shared/Action/Components/ActionComponent.h"
//#include "Header/DebugHeader.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = false;

    // 컴포넌트 생성
    StatsComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("StatsComponent"));
    ActionComponent = CreateDefaultSubobject<UActionComponent>(TEXT("ActionComponent"));
    BuffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
    
    // GameplayTag 초기화
    // InvincibleTag = FGameplayTag::RequestGameplayTag(FName("Character.State.Invincible"));
}



void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();
    
}

void ABaseCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    
    // if (ActionComponent) { ActionComponent->SetOwnerCharacter(this); }
}

void ABaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 타이머 정리
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(DeathTimer);
        GetWorld()->GetTimerManager().ClearTimer(HitStopTimer);
    }

    Super::EndPlay(EndPlayReason);
}

void ABaseCharacter::BeginDestroy()
{
    // 델리게이트 정리
    OnCharacterDeath.Clear();
    
    Super::BeginDestroy();
}

#pragma region Death

void ABaseCharacter::Die()
{
    if (bIsDead)
    {
        return;
    }

    bIsDead = true;
    
    // 무적 상태 추가
    // AddCharacterTag(InvincibleTag);
    
    // 데스 몽타주 재생
    if (DeathMontage)
    {
        PlayMontage(DeathMontage);
    }

    // 델리게이트 브로드캐스트
    OnCharacterDeath.Broadcast(this);

    // 캐릭터 이동 비활성화
    if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
    {
        MovementComp->DisableMovement();
    }

    // 충돌 비활성화
    if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
    {
        CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    // 타이머 설정
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            DeathTimer,
            this,
            &ABaseCharacter::FinishDeath,
            DeathTime,
            false
        );
    }
}

void ABaseCharacter::FinishDeath()
{
    // 블루프린트에서 오버라이드 가능
}

void ABaseCharacter::DestroyCharacter()
{
    Destroy();
}

#pragma endregion

#pragma region Movement

void ABaseCharacter::SetMaxWalkSpeed(float Speed) const
{
    if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
    {
        MovementComp->MaxWalkSpeed = Speed;
    }
}

void ABaseCharacter::SetJumpVelocity(float Velocity) const
{
    if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
    {
        MovementComp->JumpZVelocity = Velocity;
    }
}

void ABaseCharacter::SetGravityScale(float Scale) const
{
    if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
    {
        MovementComp->GravityScale = Scale;
    }
}

#pragma endregion

#pragma region Montage

void ABaseCharacter::PlayMontage(UAnimMontage* Montage) const
{
    if (!Montage || !GetMesh() || !GetMesh()->GetAnimInstance())
    {
        return;
    }

    GetMesh()->GetAnimInstance()->Montage_Play(Montage);
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, FName SectionName) const
{
    if (!Montage || !GetMesh() || !GetMesh()->GetAnimInstance())
    {
        return;
    }

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (SectionName.IsNone())
    {
        AnimInstance->Montage_Play(Montage);
    }
    else
    {
        AnimInstance->Montage_Play(Montage);
        AnimInstance->Montage_JumpToSection(SectionName, Montage);
    }
}

void ABaseCharacter::PlayMontageNextSection(UAnimMontage* Montage, FName SectionName) const
{
    if (!Montage || !GetMesh() || !GetMesh()->GetAnimInstance())
    {
        return;
    }

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance->Montage_IsPlaying(Montage))
    {
        AnimInstance->Montage_SetNextSection(
            AnimInstance->Montage_GetCurrentSection(Montage),
            SectionName,
            Montage
        );
    }
}

void ABaseCharacter::PlayMontageWithEndDelegate(UAnimMontage* Montage, FOnMontageEnded& Delegate) const
{
    if (!Montage || !GetMesh() || !GetMesh()->GetAnimInstance())
    {
        return;
    }

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    AnimInstance->Montage_Play(Montage);
    AnimInstance->Montage_SetEndDelegate(Delegate, Montage);
}

void ABaseCharacter::PlayMontageWithBlendingOutDelegate(UAnimMontage* Montage, FOnMontageBlendingOutStarted& Delegate) const
{
    if (!Montage || !GetMesh() || !GetMesh()->GetAnimInstance())
    {
        return;
    }

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    AnimInstance->Montage_Play(Montage);
    AnimInstance->Montage_SetBlendingOutDelegate(Delegate, Montage);
}

void ABaseCharacter::SetCurrentMontageRate(float NewRate) const
{
    if (!GetMesh() || !GetMesh()->GetAnimInstance())
    {
        return;
    }

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage())
    {
        AnimInstance->Montage_SetPlayRate(CurrentMontage, NewRate);
    }
}

#pragma endregion

#pragma region Tag

void ABaseCharacter::AddCharacterTag(const FGameplayTag& TagToAdd)
{
    if (TagToAdd.IsValid())
    {
        CharacterTags.AddTag(TagToAdd);
    }
}

void ABaseCharacter::RemoveCharacterTag(const FGameplayTag& TagToRemove)
{
    if (TagToRemove.IsValid())
    {
        CharacterTags.RemoveTag(TagToRemove);
    }
}

FGameplayTag ABaseCharacter::FindCharacterTagByName(FName TagNameToFind) const
{
    for (const FGameplayTag& Tag : CharacterTags)
    {
        if (Tag.GetTagName() == TagNameToFind)
        {
            return Tag;
        }
    }
    return FGameplayTag();
}

bool ABaseCharacter::HasCharacterTag(const FGameplayTag& TagToCheck) const
{
    return CharacterTags.HasTag(TagToCheck);
}

bool ABaseCharacter::HasAnyCharacterTags(const FGameplayTagContainer& TagsToCheck) const
{
    return CharacterTags.HasAny(TagsToCheck);
}

bool ABaseCharacter::HasAllCharacterTags(const FGameplayTagContainer& TagsToCheck) const
{
    return CharacterTags.HasAll(TagsToCheck);
}

#pragma endregion

#pragma region Combat

void ABaseCharacter::ApplyHitStop(float Duration)
{
    if (Duration <= 0.0f)
    {
        return;
    }

    // 기존 히트스톱 타이머 클리어
    if (GetWorld() && HitStopTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(HitStopTimer);
    }

    // 애니메이션 정지
    if (GetMesh() && GetMesh()->GetAnimInstance())
    {
        GetMesh()->GetAnimInstance()->Montage_Pause();
    }

    // 이동 정지
    if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
    {
        MovementComp->Velocity = FVector::ZeroVector;
    }

    // 히트스톱 종료 타이머 설정
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            HitStopTimer,
            this,
            &ABaseCharacter::EndHitStop,
            Duration,
            false
        );
    }
}

void ABaseCharacter::EndHitStop()
{
    // 애니메이션 재개
    if (GetMesh() && GetMesh()->GetAnimInstance())
    {
        GetMesh()->GetAnimInstance()->Montage_Resume(nullptr);
    }

    // 타이머 클리어
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(HitStopTimer);
    }
}


#pragma endregion

#pragma region Audio_VFX

void ABaseCharacter::PlayFootstepSound()
{
    if (FootstepSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            FootstepSound,
            GetActorLocation()
        );
    }
}

void ABaseCharacter::SpawnFootstepEffect()
{
    if (FootstepEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            FootstepEffect,
            GetActorLocation(),
            GetActorRotation()
        );
    }
}

void ABaseCharacter::PlayHitSound()
{
    if (HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            HitSound,
            GetActorLocation()
        );
    }
}

void ABaseCharacter::SpawnHitEffect(const FVector& Location, const FVector& Normal)
{
    if (HitParticles)
    {
        FRotator EffectRotation = Normal.Rotation();
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            HitParticles,
            Location,
            EffectRotation
        );
    }
}

#pragma endregion