#pragma once

#include "CoreMinimal.h"
#include "MapGraph.h"
#include "GameFramework/Actor.h"
#include "MapGraphGenerator.generated.h"

UENUM(BlueprintType)
enum class EMapLayout : uint8
{
	Rectangular,
	Circular,
	ZShape,
};

UCLASS()
class ARPG_MAP_API AMapGraphGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	
protected:
	UPROPERTY(EditDefaultsOnly)
	int32 Rows { 5 };
	UPROPERTY(EditDefaultsOnly)
	int32 Columns { 6 };

	// Main Path parameters
	UPROPERTY(EditDefaultsOnly)
	FMapGraphCoord MainPathStart { Rows / 2, 0 };
	UPROPERTY(EditDefaultsOnly)
	int32 MainPathLength { 6 };
	
	UPROPERTY(EditDefaultsOnly)
	EMapLayout MapLayout {EMapLayout::Rectangular };
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Pass by copy because FMapGraph created is a local variable. Not expensive with RVO
	FMapGraph BuildMapGraph();

	void GenerateMainPath(FMapGraph& MapGraph);

	void GenerateBranches(FMapGraph& MapGraph, FMapGraphCoord GraphNodeStart, int32 BranchLength, EMapConnectorDir BranchDirection);

	void AddConnectors(FMapGraphNode& GraphNode, FMapGraphCoord GraphNodeCoord);
};
