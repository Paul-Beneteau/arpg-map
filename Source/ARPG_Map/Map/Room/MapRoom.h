#pragma once

#include "CoreMinimal.h"
#include "MapRoomConnector.h"
#include "GameFramework/Actor.h"
#include "MapRoom.generated.h"

// Small chunk of a map which can be connected to other room to create a larger level
UCLASS()
class ARPG_MAP_API AMapRoom : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UMapRoomConnector>> Connectors;
	
	AMapRoom();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

	virtual void BeginPlay() override;
};
