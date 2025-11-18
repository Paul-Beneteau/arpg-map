#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MapPlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class ARPG_MAP_API AMapPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMapPlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:	
	/** Time Threshold to know if it was a  short click to set destination */
	float ClickToDestinationThreshold = 0.3f;	
	float SetDestinationTriggerDuration = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	const UInputAction* InputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	UInputMappingContext* DefaultInputContext;
	
	virtual void BeginPlay() override;

	void OnInputStarted();

	void OnSetDestinationTriggered();

	void OnSetDestinationReleased();
};
