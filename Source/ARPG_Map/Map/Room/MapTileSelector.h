#pragma once

#include "CoreMinimal.h"
#include "ARPG_Map/Map/Types/MapTypes.h"
#include "GameFramework/Actor.h"
#include "MapTileSelector.generated.h"

struct FMapGraphCell;
struct FMapGraph;
class AMapTile;
class UMapTileTemplate;

// Tile class and world location to spawn the tile
USTRUCT()
struct FMapTileSpawnData
{
	GENERATED_BODY()
    
	UPROPERTY()
	TSubclassOf<AMapTile> TileClass;
	
	FMapGraphCoord Coord;

	FORCEINLINE FVector GetWorldLocation(int32 TileSize) const 
	{
		return FVector(-Coord.Row * TileSize, Coord.Column * TileSize, 0);
	}
};

// Selects tile to spawn that match graph cell connection requirements. Uses weighted random selection from candidates tile templates. 
UCLASS(ClassGroup=(Map), meta=(BlueprintSpawnableComponent))
class ARPG_MAP_API UMapTileSelector : public UActorComponent
{
	GENERATED_BODY()

public:
	// Selects a random valid tile for each used cell in the graph.
	TArray<FMapTileSpawnData> SelectMapGraphTiles(const FMapGraph& Graph);
	
protected:
	// Tile templates available for selection.
	UPROPERTY(EditDefaultsOnly, Category = "Tile Selection")
	TArray<UMapTileTemplate*> TileTemplates;

	// Picks a tile template using weighted random selection from valid tile templates
	const UMapTileTemplate* PickTileTemplate(const FMapGraphCell& Cell);

	// Get possible valid tile templates for the graph cell
	TArray<const UMapTileTemplate*> GetCandidateTileTemplates(const FMapGraphCell& Cell);
};
