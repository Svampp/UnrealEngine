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
    }

    if (OwningCharacter)
    {
        FVector Velocity = OwningCharacter->GetVelocity();
        FVector Forward = OwningCharacter->GetActorForwardVector();
        FVector Right = OwningCharacter->GetActorRightVector();

        Speed = Velocity.Size2D();

        FVector NormalizedVelocity = Velocity.GetSafeNormal2D();

        DirectionX = FVector::DotProduct(NormalizedVelocity, Forward);
        DirectionY = FVector::DotProduct(NormalizedVelocity, Right);

        bIsInAir = OwningCharacter->GetCharacterMovement()->IsFalling();
    }
}