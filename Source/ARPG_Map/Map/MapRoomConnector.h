#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "MapRoomConnector.generated.h"

class AMapRoom;

UENUM(BlueprintType)
enum class EMapConnectorDir : uint8
{
	North,
	East,
	South,
	West
};

// Connector representing an entry between two rooms.
UCLASS(ClassGroup=(Map), meta=(BlueprintSpawnableComponent))
class ARPG_MAP_API UMapRoomConnector : public UArrowComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapConnectorDir Direction = EMapConnectorDir::North;
	
	void DrawDebug(const int32 RoomColumn, const int32 RoomRow);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Width = 300.f;

	bool bUsed = false;
};
