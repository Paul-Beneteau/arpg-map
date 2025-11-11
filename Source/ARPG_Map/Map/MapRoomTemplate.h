#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MapRoomTemplate.generated.h"

class AMapRoom;

// Template use to generate room
UCLASS()
class ARPG_MAP_API UMapRoomTemplate : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AMapRoom> RoomClass;
};
