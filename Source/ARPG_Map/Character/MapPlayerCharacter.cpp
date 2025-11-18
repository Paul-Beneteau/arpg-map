#include "MapPlayerCharacter.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AMapPlayerCharacter::AMapPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMapPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMapPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMapPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(InputAction && DefaultInputContext)
	
	const APlayerController* PlayerController { GetController<APlayerController>() };
	check(PlayerController);	
	const ULocalPlayer* LocalPlayer { PlayerController->GetLocalPlayer() };
	check(LocalPlayer);	

	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem { LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() })
	{
		InputSubsystem->AddMappingContext(DefaultInputContext, 0);
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent { Cast<UEnhancedInputComponent>(InputComponent) })
	{
		// Bind movement input
		EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Started, this, &AMapPlayerCharacter::OnInputStarted);
		EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &AMapPlayerCharacter::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Completed, this, &AMapPlayerCharacter::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Canceled, this, &AMapPlayerCharacter::OnSetDestinationReleased);
	}
}

void AMapPlayerCharacter::OnInputStarted()
{
	GetController()->StopMovement();	
}

// Move toward destination while input is triggered
void AMapPlayerCharacter::OnSetDestinationTriggered()
{
	SetDestinationTriggerDuration += GetWorld()->GetDeltaSeconds();

	if (APlayerController* PlayerController { Cast<APlayerController>(GetController()) })
	{
		// Get destination location at the cursor position
		FHitResult Hit;
		if (PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
		{
			// Move towards destination
			FVector WorldDirection { (Hit.Location - GetActorLocation()).GetSafeNormal() };
			AddMovementInput(WorldDirection, 1.0, false);
		}
	}
}

// Move to destination if this was a short click
void AMapPlayerCharacter::OnSetDestinationReleased()
{
	// If this is a short click
	if (SetDestinationTriggerDuration <= ClickToDestinationThreshold)
	{
		// Get destination location at the cursor position 
		if (APlayerController* PlayerController { Cast<APlayerController>(GetController()) })
		{
			FHitResult Hit;
			if (PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
			{
				// Move to destination
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), Hit.Location);
			}
		}		
	}

	SetDestinationTriggerDuration = 0.f;
}
