#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Graph/MapGraph.h"
#include "MapGenerator.generated.h"

class AMapTile;
struct FMapTileSpawnData;
class UMapTileSelector;
class UMapGraphGenerator;
struct FMapGraph;

// Procedural map generator. Generate a graph with GraphGenerator, then select tile to spawn with TileSelector, then spawned tiles.
UCLASS()
class ARPG_MAP_API AMapGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	AMapGenerator();

	// Generate a procedural map from layout config data table assigned to GraphGenerator.
	UFUNCTION(BlueprintCallable)
	void GenerateMap();
	// Destroy spawned tiles
	UFUNCTION(BlueprintCallable)
	void ClearMap();

	// Show the graph used to generate the current map
	UFUNCTION(BlueprintCallable)
	void ShowGraph() const;
	UFUNCTION(BlueprintCallable)
	void HideGraph() const;

	UFUNCTION(BlueprintCallable)
	void ShowLayoutInfo();
	UFUNCTION(BlueprintCallable)
	void HideLayoutInfo();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Map Generation")
	TObjectPtr<UMapGraphGenerator> GraphGenerator;
    
	UPROPERTY(EditDefaultsOnly, Category = "Map Generation")
	TObjectPtr<UMapTileSelector> TileSelector;

	UPROPERTY(EditDefaultsOnly, Category = "Map Generation")
	int32 TileSize = 1000;

	UPROPERTY()
	TArray<TObjectPtr<AMapTile>> SpawnedTiles;

	FMapGraph CachedMapGraph;

	UPROPERTY(EditDefaultsOnly, Category = "Map Generation")
	TSubclassOf<UUserWidget> LayoutWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> LayoutWidget ;
	
	void MovePlayerToStart(const FMapGraph& Graph);
};
