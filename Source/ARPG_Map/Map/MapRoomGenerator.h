#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapRoomGenerator.generated.h"

class AMapRoom;
class UMapRoomConnector;
struct FConnector;
class UMapRoomTemplate;

// Generate procedural map assembling room together
UCLASS()
class ARPG_MAP_API AMapRoomGenerator : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere)
	TArray<UMapRoomTemplate*> RoomTemplates;

	// TODO: Moves data in a data asset
	UPROPERTY(EditDefaultsOnly)
	int32 ColumnCount = 2;
	UPROPERTY(EditDefaultsOnly)
	int32 RowCount = 2;
	
	virtual void BeginPlay() override;

	//UMapRoomTemplate GetRequiredRoomTemplateFromGraphNode(const int32 Row, const int32 Column);
	
	// Move the room so that his ConnectorToAttach snap to TargetConnector
	void AttachRoomToConnector(AMapRoom* Room, const UMapRoomConnector* TargetConnector, const UMapRoomConnector* ConnectorToAttach);

	void DrawRoomDebug(const AMapRoom* Room, const int32 Row, const int32 Column) const;

	// Check if connector is inside map borders
	bool IsConnectorValid(const UMapRoomConnector* Connector, const int32 Row, const int32 Column) const;

	const UMapRoomConnector* GetFirstValidConnector(const TArray<UMapRoomConnector*>& Connectors, const int32 Row, const int32 Column);

	const UMapRoomTemplate* GetRandomValidRoomTemplate(const int32 Row, const int32 Column);
};
