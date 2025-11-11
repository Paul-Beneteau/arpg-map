#include "MapGraphGenerator.h"

#include "MapGraph.h"
#include "MapRoomConnector.h"

void AMapGraphGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	FMapGraph MapGraph = BuildMapGraph();
	
	MapGraph.Print(GetWorld());
}

FMapGraph AMapGraphGenerator::BuildMapGraph()
{
	FMapGraph MapGraph(Rows, Columns);

	GenerateMainPath(MapGraph);

	// Generate branches from the main path
	for (int32 MainPathIndex = 0; MainPathIndex < MainPathLength; ++MainPathIndex)
	{
		int32 CenterRow { Rows / 2 };
		
		GenerateBranches(MapGraph, FMapGraphCoord(CenterRow, MainPathIndex), 1, EMapConnectorDir::North);
	}

	return MapGraph;
}

void AMapGraphGenerator::GenerateMainPath(FMapGraph& MapGraph)
{
	for (int32 MainPathIndex = 0; MainPathIndex < MainPathLength; ++MainPathIndex)
	{
		FMapGraphNode MainPathNode;

		// TODO: fix AddConnectors as it adds every connector for now
		AddConnectors(MainPathNode, FMapGraphCoord(MainPathStart.Row, MainPathStart.Column + MainPathIndex));

		if (MainPathIndex == 0)
			MainPathNode.Role = "MainPathStart";
		else if (MainPathIndex == MainPathLength - 1)
			MainPathNode.Role = "MainPathEnd";
		else
			MainPathNode.Role = "MainPath";
		
		MainPathNode.Theme = "StonePath";

		MapGraph.At(MainPathStart.Row, MainPathIndex) = MainPathNode;
	}
}

void AMapGraphGenerator::GenerateBranches(FMapGraph& MapGraph, FMapGraphCoord GraphNodeStart, int32 BranchLength, EMapConnectorDir BranchDirection)
{
	for (int32 BranchIndex = 1; BranchIndex <= BranchLength; ++BranchIndex)
	{
		FMapGraphNode MainPathNode;

		AddConnectors(MainPathNode, FMapGraphCoord(GraphNodeStart.Row, GraphNodeStart.Column));		
		MainPathNode.Role = "Side";
		MainPathNode.Theme = "Plain";

		// Place branch graph node according to his direction
		int32 BranchRow = GraphNodeStart.Row;
		int32 BranchColumn = GraphNodeStart.Column;
		
		if (BranchDirection == EMapConnectorDir::West)
			BranchColumn = GraphNodeStart.Column - BranchIndex;
		else if (BranchDirection == EMapConnectorDir::East)
			BranchColumn = GraphNodeStart.Column + BranchIndex;
		else if (BranchDirection == EMapConnectorDir::North)
			BranchRow = GraphNodeStart.Row + BranchIndex;
		else if (BranchDirection == EMapConnectorDir::South)
			BranchRow = GraphNodeStart.Row - BranchIndex;

		MapGraph.At(BranchRow, BranchColumn) = MainPathNode;
	}
}

void AMapGraphGenerator::AddConnectors(FMapGraphNode& GraphNode, FMapGraphCoord GraphNodeCoord)
{
	if (GraphNodeCoord.Column > 0)
		GraphNode.Connectors.Add(EMapConnectorDir::West);		
	if (GraphNodeCoord.Column < Columns - 1)
		GraphNode.Connectors.Add(EMapConnectorDir::East);		
	if (GraphNodeCoord.Row > 0)
		GraphNode.Connectors.Add(EMapConnectorDir::North);		
	if (GraphNodeCoord.Row < Rows - 1)		
		GraphNode.Connectors.Add(EMapConnectorDir::South);
}
