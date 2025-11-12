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
	FMapGraph CachedMapGraph;
	
	UPROPERTY(EditDefaultsOnly)
	int32 Rows { 5 };
	UPROPERTY(EditDefaultsOnly)
	int32 Columns { 6 };

	UPROPERTY(EditDefaultsOnly)
	FMapLayoutConfig LayoutConfig;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Pass by copy because FMapGraph created is a local variable. Not expensive with RVO
	void BuildMapGraph();

	void GenerateMainPath();

	void GenerateSegment(FMapGraphCoord Anchor, const FMapSegment& Segment);
	
	void AddConnectors(FMapGraphNode& GraphNode, FMapGraphCoord GraphNodeCoord);

	void GenerateBranchesFromMainPath(const FBranchRule& BranchRule);
};
