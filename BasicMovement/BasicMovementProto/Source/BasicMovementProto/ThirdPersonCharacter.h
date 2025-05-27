#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ThirdPersonCharacter.generated.h"

// Movement states for the character
UENUM(BlueprintType)
enum class EMovementState : uint8
{
    MS_Walking     UMETA(DisplayName = "Walking"),
    MS_Running     UMETA(DisplayName = "Running"),
    MS_Jumping     UMETA(DisplayName = "Jumping"),
    MS_Crawling    UMETA(DisplayName = "Crawling"),
    MS_Swimming    UMETA(DisplayName = "Swimming"),
    MS_Climbing    UMETA(DisplayName = "Climbing")
};

UCLASS()
class BASICMOVEMENTPROTO_API AThirdPersonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AThirdPersonCharacter();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Movement parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float WalkSpeed = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float RunSpeed = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float CrawlSpeed = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float SwimSpeed = 250.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float ClimbSpeed = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float JumpVelocity = 420.0f;

    // Current movement state
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    EMovementState CurrentMovementState;

    // Is the character currently sprinting?
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool bIsSprinting;

    // Is the character currently crawling?
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool bIsCrawling;

    // Is the character in water?
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool bIsInWater;

    // Is the character climbing?
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool bIsClimbing;

    // Surface normal when climbing
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    FVector ClimbingSurfaceNormal;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Movement input handlers
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);

    // Action input handlers
    void StartJumping();
    void StopJumping();
    void StartSprinting();
    void StopSprinting();
    void ToggleCrawling();

    // Movement state updates
    void UpdateMovementState();
    void UpdateCharacterMovement();

    // Environmental checks
    bool CheckForWater();
    bool CheckForClimbableSurface();

    // Climbing specific functions
    void StartClimbing();
    void StopClimbing();
    void UpdateClimbingMovement(float DeltaTime);
};