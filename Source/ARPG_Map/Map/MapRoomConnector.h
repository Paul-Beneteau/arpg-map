#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "Graph/MapLayout.h"
#include "MapRoomConnector.generated.h"

class AMapRoom;

// Connector representing an entry between two rooms.
UCLASS(ClassGroup=(Map), meta=(BlueprintSpawnableComponent))
class ARPG_MAP_API UMapRoomConnector : public UArrowComponent
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
