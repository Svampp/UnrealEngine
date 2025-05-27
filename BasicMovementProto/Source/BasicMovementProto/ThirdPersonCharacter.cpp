#include "ThirdPersonCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AThirdPersonCharacter::AThirdPersonCharacter()
{
    // Set this character to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;

    // Initialize movement state
    CurrentMovementState = EMovementState::MS_Walking;
    bIsSprinting = false;
    bIsCrawling = false;
    bIsInWater = false;
    bIsClimbing = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = JumpVelocity;
    GetCharacterMovement()->AirControl = 0.2f;
    GetCharacterMovement()->GravityScale = 1.0f;
}

// Called when the game starts or when spawned
void AThirdPersonCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Set initial movement speed
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// Called every frame
void AThirdPersonCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update environmental checks
    bIsInWater = CheckForWater();
    bIsClimbing = CheckForClimbableSurface();

    // Update movement state based on environment and input
    UpdateMovementState();

    // Handle climbing movement if needed
    if (bIsClimbing)
    {
        UpdateClimbingMovement(DeltaTime);
    }
}

// Called to bind functionality to input
void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Bind movement axis
    PlayerInputComponent->BindAxis("MoveForward", this, &AThirdPersonCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AThirdPersonCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AThirdPersonCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AThirdPersonCharacter::LookUp);

    // Bind action inputs
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AThirdPersonCharacter::StartJumping);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &AThirdPersonCharacter::StopJumping);
    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AThirdPersonCharacter::StartSprinting);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AThirdPersonCharacter::StopSprinting);
    PlayerInputComponent->BindAction("Crawl", IE_Pressed, this, &AThirdPersonCharacter::ToggleCrawling);
}

// Movement input handlers
void AThirdPersonCharacter::MoveForward(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        // Find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // Get forward vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        // Handle climbing movement differently
        if (bIsClimbing)
        {
            // Move along the climbing surface
            AddMovementInput(FVector::VectorPlaneProject(Direction, ClimbingSurfaceNormal), Value);
        }
        else
        {
            // Normal movement
            AddMovementInput(Direction, Value);
        }
    }
}

void AThirdPersonCharacter::MoveRight(float Value)
{
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        // Find out which way is right
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // Get right vector 
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // Handle climbing movement differently
        if (bIsClimbing)
        {
            // Move along the climbing surface
            AddMovementInput(FVector::VectorPlaneProject(Direction, ClimbingSurfaceNormal), Value);
        }
        else
        {
            // Normal movement
            AddMovementInput(Direction, Value);
        }
    }
}

void AThirdPersonCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void AThirdPersonCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

// Action input handlers
void AThirdPersonCharacter::StartJumping()
{
    if (!bIsCrawling && !bIsInWater && !bIsClimbing)
    {
        Jump();
    }
    else if (bIsInWater)
    {
        // Swimming "jump" - push upward in water
        LaunchCharacter(FVector(0, 0, JumpVelocity * 0.5f), false, false);
    }
}

void AThirdPersonCharacter::StopJumping()
{
    StopJumping();
}

void AThirdPersonCharacter::StartSprinting()
{
    if (!bIsCrawling && !bIsInWater && !bIsClimbing)
    {
        bIsSprinting = true;
        UpdateCharacterMovement();
    }
}

void AThirdPersonCharacter::StopSprinting()
{
    bIsSprinting = false;
    UpdateCharacterMovement();
}

void AThirdPersonCharacter::ToggleCrawling()
{
    // Can't crawl while swimming or climbing
    if (!bIsInWater && !bIsClimbing)
    {
        bIsCrawling = !bIsCrawling;

        // Adjust capsule component size for crawling
        if (bIsCrawling)
        {
            GetCapsuleComponent()->SetCapsuleHalfHeight(GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * 0.5f);
        }
        else
        {
            GetCapsuleComponent()->SetCapsuleHalfHeight(GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() * 2.0f);
        }

        UpdateCharacterMovement();
    }
}

// Movement state updates
void AThirdPersonCharacter::UpdateMovementState()
{
    if (bIsClimbing)
    {
        CurrentMovementState = EMovementState::MS_Climbing;
    }
    else if (bIsInWater)
    {
        CurrentMovementState = EMovementState::MS_Swimming;
    }
    else if (GetCharacterMovement()->IsFalling())
    {
        CurrentMovementState = EMovementState::MS_Jumping;
    }
    else if (bIsCrawling)
    {
        CurrentMovementState = EMovementState::MS_Crawling;
    }
    else if (bIsSprinting)
    {
        CurrentMovementState = EMovementState::MS_Running;
    }
    else
    {
        CurrentMovementState = EMovementState::MS_Walking;
    }
}

void AThirdPersonCharacter::UpdateCharacterMovement()
{
    switch (CurrentMovementState)
    {
    case EMovementState::MS_Walking:
        GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
        GetCharacterMovement()->GravityScale = 1.0f;
        break;

    case EMovementState::MS_Running:
        GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
        GetCharacterMovement()->GravityScale = 1.0f;
        break;

    case EMovementState::MS_Jumping:
        // Jumping parameters are set in constructor
        break;

    case EMovementState::MS_Crawling:
        GetCharacterMovement()->MaxWalkSpeed = CrawlSpeed;
        GetCharacterMovement()->GravityScale = 1.0f;
        break;

    case EMovementState::MS_Swimming:
        GetCharacterMovement()->MaxWalkSpeed = SwimSpeed;
        GetCharacterMovement()->GravityScale = 0.3f; // Reduced gravity in water
        break;

    case EMovementState::MS_Climbing:
        GetCharacterMovement()->MaxWalkSpeed = ClimbSpeed;
        GetCharacterMovement()->GravityScale = 0.0f; // No gravity while climbing
        break;
    }
}

// Environmental checks
bool AThirdPersonCharacter::CheckForWater()
{
    // Simple water check - in a real game you'd want a more robust system
    TArray<AActor*> OverlappingActors;
    GetCapsuleComponent()->GetOverlappingActors(OverlappingActors, TSubclassOf<AActor>());

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor->ActorHasTag("Water"))
        {
            return true;
        }
    }

    return false;
}

bool AThirdPersonCharacter::CheckForClimbableSurface()
{
    // Check for climbable surface in front of the character
    FVector Start = GetActorLocation();
    FVector Forward = GetActorForwardVector();
    FVector End = Start + (Forward * 100.0f); // Check 100cm in front

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        Start,
        End,
        ECC_Visibility,
        CollisionParams
    );

    if (bHit && HitResult.GetActor()->ActorHasTag("Climbable"))
    {
        ClimbingSurfaceNormal = HitResult.ImpactNormal;
        return true;
    }

    return false;
}

// Climbing specific functions
void AThirdPersonCharacter::StartClimbing()
{
    if (bIsClimbing)
    {
        // Disable standard movement physics
        GetCharacterMovement()->SetMovementMode(MOVE_Flying);
    }
}

void AThirdPersonCharacter::StopClimbing()
{
    if (!bIsClimbing)
    {
        // Re-enable standard movement physics
        GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    }
}

void AThirdPersonCharacter::UpdateClimbingMovement(float DeltaTime)
{
    // Apply a small force toward the climbing surface to keep the character attached
    FVector ForceDirection = -ClimbingSurfaceNormal * 500.0f;
    GetCharacterMovement()->AddForce(ForceDirection);

    // Counteract gravity
    GetCharacterMovement()->Velocity.Z = 0.0f;
}