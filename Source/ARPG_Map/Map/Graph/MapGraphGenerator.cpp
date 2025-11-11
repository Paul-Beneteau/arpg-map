#include "MapGraphGenerator.h"

#include "MapGraph.h"

void AMapGraphGenerator::BeginPlay()
{
	Super::BeginPlay();

	// TODO: load parameter from data asset
	LayoutConfig.MainPathStart = FMapGraphCoord(Rows / 2, 0);
	LayoutConfig.MainPathLength = 6;
	LayoutConfig.MinSegmentLength = 1;
	LayoutConfig.MaxSegmentLength = 3;
	
	FLayoutDefinition MapLayout;
	MapLayout.Layout = EMapLayout::L;

	MapLayout.PathSegments =
	{
		FMapSegment(EMapDirection::East, TArray<FMapSegmentSection>( { FMapSegmentSection(2, EMapRole::MainPath, "Plain") })), 
		FMapSegment(EMapDirection::North, TArray<FMapSegmentSection>( { FMapSegmentSection(2, EMapRole::MainPath, "Plain") })),
		FMapSegment(EMapDirection::East, TArray<FMapSegmentSection>( { FMapSegmentSection(1, EMapRole::MainPath, "Plain") })),
		FMapSegment(EMapDirection::East, TArray<FMapSegmentSection>( { FMapSegmentSection(1, EMapRole::MainPathEnd, "Plain") })), 
	};
	
	LayoutConfig.AvailableLayouts.Add(MapLayout);
	
	FMapGraph MapGraph = BuildMapGraph();
	
	MapGraph.Print(GetWorld());
}

FMapGraph AMapGraphGenerator::BuildMapGraph()
{
	FMapGraph MapGraph(Rows, Columns);

	GenerateMainPath(MapGraph);

	FMapGraphCoord MainPathIndexCoord = LayoutConfig.MainPathStart;

	for (const FMapSegment& Segment : LayoutConfig.AvailableLayouts[0].PathSegments)
	{
		for (int32 SegmentIndex = 0;  SegmentIndex < Segment.GetLength(); ++SegmentIndex)
		{
			FMapSegment BranchSegment(EMapDirection::South, TArray<FMapSegmentSection>( { FMapSegmentSection(2, EMapRole::None, "Plain") }));
		
			GenerateSegment(MapGraph, MainPathIndexCoord, BranchSegment);

			MainPathIndexCoord.Step(Segment.Direction, 1);
		}
	}

	return MapGraph;
}

void AMapGraphGenerator::GenerateMainPath(FMapGraph& MapGraph)
{
	FLayoutDefinition MapLayout = LayoutConfig.AvailableLayouts[FMath::RandRange(0, LayoutConfig.AvailableLayouts.Num() - 1)];
	
	FMapGraphCoord SegmentStart = LayoutConfig.MainPathStart;

	MapGraph.At(SegmentStart) = FMapGraphNode(EMapRole::MainPathStart, "Plain");
	
	for (const FMapSegment& PathSegment : MapLayout.PathSegments)
	{		
		GenerateSegment(MapGraph, SegmentStart, PathSegment);

		SegmentStart.Step(PathSegment.Direction, PathSegment.GetLength());
	}
}

void AMapGraphGenerator::GenerateSegment(FMapGraph& MapGraph, FMapGraphCoord SegmentStart, const FMapSegment& Segment)
{
	for (const FMapSegmentSection& SegmentSection : Segment.Sections)
	{
		for (int32 SectionIndex = 1; SectionIndex <= SegmentSection.Length; ++SectionIndex)
		{
			SegmentStart.Step(Segment.Direction, 1);
			
			if (!MapGraph.IsValidCoord(SegmentStart))
			{
				UE_LOG(LogTemp, Warning, TEXT("AMapGraphGenerator: Segment is out of bounds!"));
				break;
			}			

			if (MapGraph.At(SegmentStart).IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("AMapGraphGenerator: Segment to generate is on an existing graph node!"));
				break;
			}			
				
			MapGraph.At(SegmentStart) = FMapGraphNode(SegmentSection.Role, SegmentSection.Theme);
		}
	}
}

void AMapGraphGenerator::AddConnectors(FMapGraphNode& GraphNode, FMapGraphCoord GraphNodeCoord)
{
	if (GraphNodeCoord.Column > 0)
		GraphNode.Connectors.Add(EMapDirection::West);		
	if (GraphNodeCoord.Column < Columns - 1)
		GraphNode.Connectors.Add(EMapDirection::East);		
	if (GraphNodeCoord.Row > 0)
		GraphNode.Connectors.Add(EMapDirection::North);		
	if (GraphNodeCoord.Row < Rows - 1)		
		GraphNode.Connectors.Add(EMapDirection::South);
}
