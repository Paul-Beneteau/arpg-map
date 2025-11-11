#pragma once

#include "CoreMinimal.h"
#include "MapGraph.h"
#include "MapLayout.h"
#include "GameFramework/Actor.h"
#include "MapGraphGenerator.generated.h"

UCLASS()
class ARPG_MAP_API AMapGraphGenerator : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly)
	int32 Rows { 5 };
	UPROPERTY(EditDefaultsOnly)
	int32 Columns { 6 };

	UPROPERTY(EditDefaultsOnly)
	FMapLayoutConfig LayoutConfig;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Pass by copy because FMapGraph created is a local variable. Not expensive with RVO
	FMapGraph BuildMapGraph();

	void GenerateMainPath(FMapGraph& MapGraph);

	void GenerateSegment(FMapGraph& MapGraph, FMapGraphCoord SegmentStart, const FMapSegment& Segment);
	
	void AddConnectors(FMapGraphNode& GraphNode, FMapGraphCoord GraphNodeCoord);
};
