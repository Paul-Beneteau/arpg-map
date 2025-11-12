#include "MapGraphGenerator.h"

#include "MapGraph.h"
#include "MapGraphUtils.h"

void AMapGraphGenerator::BeginPlay()
{
	Super::BeginPlay();

	CachedMapGraph.Resize(Rows, Columns);
	
	// TODO: load parameter from data asset
	LayoutConfig.MainPathStart = FMapGraphCoord(Rows / 2, 0);
	
	FLayoutDefinition MapLayout;
	MapLayout.Layout = EMapLayout::L;

	MapLayout.MainPathSegments =
	{
		FMapSegment(EMapDirection::East,TArray<FMapSegmentSection>({
				FMapSegmentSection(3, EMapRole::MainPath, "Plain"),
			})),
		
		FMapSegment(EMapDirection::North, TArray<FMapSegmentSection>( {
			FMapSegmentSection(2, EMapRole::MainPath, "Plain")
		})),
		
		FMapSegment(EMapDirection::East, TArray<FMapSegmentSection>({
			FMapSegmentSection(2, EMapRole::MainPath, "Plain"),
		})),

	};
	
	LayoutConfig.AvailableLayouts.Add(MapLayout);

	FBranchRule BranchRule(EMapBranchDirection::Right, 1, 1.f);
	BranchRule.Sections =
	{
		FMapSegmentSection(2, EMapRole::None, "Plain"),
	};

	LayoutConfig.BranchRules.Add(BranchRule);
	
	BuildMapGraph();
	
	CachedMapGraph.Print(GetWorld());
}

void AMapGraphGenerator::BuildMapGraph()
{
	GenerateMainPath();

	for (FBranchRule& BranchRule : LayoutConfig.BranchRules)
		GenerateBranchesFromMainPath(BranchRule);
}

void AMapGraphGenerator::GenerateMainPath()
{
	FLayoutDefinition MapLayout = LayoutConfig.AvailableLayouts[FMath::RandRange(0, LayoutConfig.AvailableLayouts.Num() - 1)];
	
	// Anchor starts one cell before the segment
	FMapGraphCoord Anchor = LayoutConfig.MainPathStart.Stepped(MapLayout.MainPathSegments[0].Direction, -1);
	
	for (const FMapSegment& Segment : MapLayout.MainPathSegments)
	{		
		GenerateSegment(Anchor, Segment);
		Anchor.Step(Segment.Direction, Segment.GetLength());
	}

	// Tag the first and last node of the main path
	CachedMapGraph.At(LayoutConfig.MainPathStart).Role = EMapRole::MainPathStart;
	CachedMapGraph.At(Anchor).Role = EMapRole::MainPathEnd;
}

void AMapGraphGenerator::GenerateBranchesFromMainPath(const FBranchRule& BranchRule)
{
	const TArray<FMapSegment>& Segments { LayoutConfig.AvailableLayouts[0].MainPathSegments };
	
	// Anchor starts one cell before the segment
	FMapGraphCoord SegmentAnchor = LayoutConfig.MainPathStart.Stepped(Segments[0].Direction, -1);
	
	for (const FMapSegment& Segment : Segments)
	{				
		for (int32 Offset = 1;  Offset <= Segment.GetLength(); Offset += BranchRule.StepInterval)
		{
			FMapGraphCoord BranchAnchor { SegmentAnchor.Stepped(Segment.Direction, Offset) };
			FMapSegment BranchSegment { GetBranchAbsoluteDirection(Segment.Direction, BranchRule.RelativeDirection), BranchRule.Sections };
			
			GenerateSegment(BranchAnchor, BranchSegment);
		}

		SegmentAnchor.Step(Segment.Direction, Segment.GetLength());
	}
}

void AMapGraphGenerator::GenerateSegment(FMapGraphCoord Anchor, const FMapSegment& Segment)
{
	FMapGraphCoord CurrentCoord = Anchor;
	
	for (const FMapSegmentSection& Section : Segment.Sections)
	{
		for (int32 Offset = 0; Offset < Section.Length; ++Offset)
		{
			CurrentCoord.Step(Segment.Direction, 1);
			
			if (!CachedMapGraph.IsValidCoord(CurrentCoord))
			{
				UE_LOG(LogTemp, Warning, TEXT("AMapGraphGenerator: Segment is out of bounds!"));
				break;
			}			

			if (CachedMapGraph.At(CurrentCoord).IsValid())
			{
				UE_LOG(LogTemp, Warning, TEXT("AMapGraphGenerator: Segment to generate is on an existing graph node!"));
				break;
			}			
				
			CachedMapGraph.At(CurrentCoord) = FMapGraphNode(Section.Role, Section.Theme);
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