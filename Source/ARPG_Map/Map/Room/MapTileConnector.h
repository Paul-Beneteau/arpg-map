#pragma once

#include "CoreMinimal.h"
#include "ARPG_Map/Map/MapGraphUtils.h"
#include "Components/ArrowComponent.h"
#include "MapTileConnector.generated.h"

class AMapTile;

// Connector representing an entry between two rooms.
UCLASS(ClassGroup=(Map), meta=(BlueprintSpawnableComponent))
class ARPG_MAP_API UMapTileConnector : public UArrowComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapDirection Direction = EMapDirection::North;
	
	void DrawDebug(const int32 RoomColumn, const int32 RoomRow);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Width = 300.f;

	bool bUsed = false;
};
