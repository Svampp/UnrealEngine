// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UThirdPersonAnimInstance::NativeInitializeAnimation()
{
    OwningCharacter = Cast<ACharacter>(TryGetPawnOwner());
}

void UThirdPersonAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    if (!OwningCharacter)
    {
        OwningCharacter = Cast<ACharacter>(TryGetPawnOwner());
        return;
    }

    FVector Velocity = OwningCharacter->GetVelocity();
    Speed = Velocity.Size2D();
    bIsInAir = OwningCharacter->GetCharacterMovement()->IsFalling();

    // Calculate movement direction
    if (Speed > 0.f)
    {
        FRotator ActorRotation = OwningCharacter->GetActorRotation();
        FRotator VelocityRotation = Velocity.ToOrientationRotator();
        FRotator DeltaRot = (VelocityRotation - ActorRotation).GetNormalized();

        DirectionX = DeltaRot.Yaw;

        // Calculate forward/backward and left/right movement amounts
        FVector ForwardVector = OwningCharacter->GetActorForwardVector();
        FVector RightVector = OwningCharacter->GetActorRightVector();

        DirectionX = FVector::DotProduct(Velocity.GetSafeNormal(), ForwardVector);
        DirectionY = FVector::DotProduct(Velocity.GetSafeNormal(), RightVector);
    }
    else
    {
        DirectionX = 0.f;
        DirectionY = 0.f;
    }
}