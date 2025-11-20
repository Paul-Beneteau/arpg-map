#pragma once

#include "CoreMinimal.h"
#include "ARPG_Map/Map/Graph/MapGraph.h"
#include "Engine/DataAsset.h"
#include "MapTileTemplate.generated.h"

enum class EMapCellType : uint8;
enum class EMapDirection : uint8;
class AMapTile;

UCLASS()
class ARPG_MAP_API UMapTileTemplate : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AMapTile> TileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapTileType Type;

	// Visual theme
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Theme;

	// Does the tile has a forward direction it must keep (e.g., a river with north forward direction, if placed on an east segment, must be rotated
	// by 90 degree clockwise). EMapDirection::None means the tile has no forward direction and can be rotated.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapDirection ForwardDirection = EMapDirection::None;
	
	bool HasConnectors(const TArray<EMapDirection>& Connectors) const;
}; 
