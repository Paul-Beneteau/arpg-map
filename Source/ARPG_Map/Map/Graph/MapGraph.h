#pragma once

#include "CoreMinimal.h"
#include "MapGraph.generated.h"

enum class EMapRole : uint8;
struct FMapGraphCoord;
enum class EMapDirection : uint8;

USTRUCT(BlueprintType)
struct FMapGraphCell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMapRole Role;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Theme;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EMapDirection> Connectors;
	
	bool IsValid() const { return !Theme.IsNone(); };
};

USTRUCT(BlueprintType)
struct FMapGraph
{
	GENERATED_BODY()

public:
	FMapGraph() = default;
	
	FMapGraph(int32 InRows, int32 InColumns);
	
	void Resize(const int32 InRows, const int32 InColumns);
	
	FMapGraphCell& At(const FMapGraphCoord& Coord);

	void Print(const UWorld* InWorld);

	int32 GetRows() const { return Rows; }

	int32 GetColumns() const { return Columns; }

	bool IsValidCoord(const FMapGraphCoord& Coord) const;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMapGraphCell> Nodes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Rows { 0 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Columns { 0 };
};