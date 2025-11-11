#include "MapRoomGenerator.h"

#include "MapRoom.h"
#include "MapRoomTemplate.h"
#include "Kismet/KismetMathLibrary.h"

void AMapRoomGenerator::BeginPlay()
{
	Super::BeginPlay();

	if (RoomTemplates.IsEmpty())
		return;
		
	// List of open connectors waiting for attachment
	TArray<const UMapRoomConnector*> OpenConnectors;

	// For every graph node
	for (int32 Row = 0; Row < RowCount; ++Row)
	{
		for (int32 Column = 0; Column < ColumnCount; ++Column)
		{
			//const UMapRoomTemplate RequiredRoomTemplate = GetRequiredRoomTemplateFromGraphNode(Row, Column);
			
			// Get template with required connectors, theme and role
			const UMapRoomTemplate* RandomRoomTemplate = GetRandomValidRoomTemplate(Row, Column);
		}
	}
}

/*UMapRoomTemplate AMapRoomGenerator::GetRequiredRoomTemplateFromGraphNode(const int32 Row, const int32 Column)
{
	AMapRoom Room;

	UMapRoomConnector Connector(EMapConnectorDir::East);	
	Room.Connectors.Add(Connector);

	UMapRoomTemplate RoomTemplate;
	RoomTemplate.RoomClass = Room.GetClass();
	
	return RoomTemplate;
}*/

/*
void AMapRoomGenerator::BeginPlay()
{
	Super::BeginPlay();

	if (RoomTemplates.IsEmpty())
		return;
		
	// List of open connectors waiting for attachment
	TArray<const UMapRoomConnector*> OpenConnectors;

	for (int32 Row = 0; Row < RowCount; ++Row)
	{
		for (int32 Column = 0; Column < ColumnCount; ++Column)
		{				
			//const UMapRoomTemplate* RandomRoomTemplate = RoomTemplates[FMath::RandRange(0, RoomTemplates.Num() - 1)];
			const UMapRoomTemplate* RandomRoomTemplate = GetRandomValidRoomTemplate(Row, Column);
			if (!RandomRoomTemplate)
			{
				UE_LOG(LogTemp, Error, TEXT("AMapRoomGenerator: No valid room template found"));
				return;
			}
			
			AMapRoom* NewRoom = GetWorld()->SpawnActor<AMapRoom>(RandomRoomTemplate->RoomClass);

			if (NewRoom->Connectors.IsEmpty())
			{
				UE_LOG(LogTemp, Error, TEXT("AMapRoomGenerator: %s has no connectors"), *NewRoom->GetName());
				continue;
			}
			
			// If this is the first room, don't move the room and add his connectors to the list of open connectors.
			if (OpenConnectors.IsEmpty())
			{
				OpenConnectors.Append(NewRoom->Connectors);
				DrawRoomDebug(NewRoom, Row, Column);					
				continue;
			}
			
			const UMapRoomConnector* TargetConnector = OpenConnectors[0];				
			UMapRoomConnector* ConnectorToAttach = NewRoom->Connectors[FMath::RandRange(0, NewRoom->Connectors.Num() - 1)];
			AttachRoomToConnector(NewRoom, TargetConnector, ConnectorToAttach);

			UE_LOG(LogTemp, Display, TEXT("Room: %d-%d: Attach %s to %s at %s"), Row, Column, *ConnectorToAttach->GetName(),
				*TargetConnector->GetName(), *TargetConnector->GetComponentLocation().ToString());
			
			DrawRoomDebug(NewRoom, Row, Column);
			
			// Remove the connector used from the list
			OpenConnectors.RemoveAt(0);

			// Add open connectors to the list
			for (const UMapRoomConnector* Connector : NewRoom->Connectors)
			{
				if (Connector != ConnectorToAttach)
					OpenConnectors.Add(Connector);
			}
		}
	}
}*/

void AMapRoomGenerator::AttachRoomToConnector(AMapRoom* Room, const UMapRoomConnector* TargetConnector, const UMapRoomConnector* ConnectorToAttach)
{
	// Compute the world location of the new room so its connector is snapped to the target connector
	const FVector ConnectorToAttachOffset = FVector::Dist(ConnectorToAttach->GetComponentLocation(), Room->GetActorLocation()) * TargetConnector->GetForwardVector();
	const FVector NewRoomLocation = TargetConnector->GetComponentLocation() + ConnectorToAttachOffset;

	// Rotate the room so its connector looks toward the target connector, then flip 180° so connectors face each other
	FRotator RoomRotation = UKismetMathLibrary::FindLookAtRotation(ConnectorToAttach->GetComponentLocation(),TargetConnector->GetComponentLocation());
	RoomRotation.Yaw += 180.f;
			
	Room->SetActorLocationAndRotation(NewRoomLocation, RoomRotation);
}

void AMapRoomGenerator::DrawRoomDebug(const AMapRoom* Room, const int32 Row, const int32 Column) const
{
	const FString Label = FString::Printf(TEXT("%d-%d"), Row, Column);
	DrawDebugString(GetWorld(), Room->GetActorLocation(), Label, nullptr, FColor::White, -1.f, true);

	for (UMapRoomConnector* Connector : Room->Connectors)
		Connector->DrawDebug(Column, Row);
}

bool AMapRoomGenerator::IsConnectorValid(const UMapRoomConnector* Connector, const int32 Row, const int32 Column) const
{
	switch (Connector->Direction)
	{
	case EMapConnectorDir::North:
		return Row > 0;
		
	case EMapConnectorDir::South:
		return Row < RowCount - 1;
		
	case EMapConnectorDir::West:
		return Column > 0;
		
	case EMapConnectorDir::East:
		return Column < ColumnCount - 1;
		
	default:
		return false;
	}
}

const UMapRoomConnector* AMapRoomGenerator::GetFirstValidConnector(const TArray<UMapRoomConnector*>& Connectors, const int32 Row, const int32 Column)
{	
	for (const UMapRoomConnector* Connector : Connectors)
	{
		if (IsConnectorValid(Connector, Row, Column))
			return Connector;
	}

	return nullptr;
};

const UMapRoomTemplate* AMapRoomGenerator::GetRandomValidRoomTemplate(const int32 Row, const int32 Column)
{
	TArray<const UMapRoomTemplate*> Valid;

	for (const UMapRoomTemplate* Template : RoomTemplates)
	{
		const AMapRoom* Defaults = Template->RoomClass.GetDefaultObject();
		for (const UMapRoomConnector* Conn : Defaults->Connectors)
		{
			if (IsConnectorValid(Conn, Row, Column))
			{
				Valid.Add(Template);
				break;
			}
		}
	}

	if (Valid.IsEmpty())
		return nullptr;

	return Valid[FMath::RandRange(0, Valid.Num() - 1)];
};
