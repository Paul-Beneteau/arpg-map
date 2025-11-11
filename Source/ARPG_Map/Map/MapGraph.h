#pragma once

#include "CoreMinimal.h"
#include "MapGraph.generated.h"

enum class EMapConnectorDir : uint8;

USTRUCT(BlueprintType)
struct FMapGraphNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EMapConnectorDir> Connectors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Theme;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Role;

	bool IsValid() const { return Connectors.Num() > 0 && !Theme.IsNone() && !Role.IsNone(); };
};

USTRUCT(BlueprintType)
struct FMapGraph
{
	GENERATED_BODY()

public:
	FMapGraph() = default;
	
	FMapGraph(int32 InRows, int32 InColumns);
	
	void Resize(const int32 InRows, const int32 InColumns);
	
	FMapGraphNode& At(const int32 Row, const int32 Column);

	void Print(const UWorld* InWorld);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMapGraphNode> Nodes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Rows { 0 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Columns { 0 };
};

USTRUCT(BlueprintType)
struct FMapGraphCoord
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Row = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Column = 0;

	FMapGraphCoord() = default;
	FMapGraphCoord(int32 InRow, int32 InColumn) : Row(InRow), Column(InColumn)	{}

	bool operator==(const FMapGraphCoord& Other) const { return Row == Other.Row && Column == Other.Column; }
};