#pragma once

#include "CoreMinimal.h"
#include "NetChannelType.generated.h"

#define CHANNEL_NUM 10
#define CONNECT_NUM 2000
#define BUFFER_SIZE 1024

UENUM(BlueprintType)
enum class ENetLinkState : uint8
{
	Listen,
	Connect
};

enum class ENetSocketType : uint8
{
	UDP,
	TCP
};

enum class ENetConnectionState : uint8
{
	UnInit,
	Login,
	Verify,
	Join
};

struct DEDICATEDSERVERUTILS_API FNetBunchHead
{
	uint32 ProtocolsNumber;
	FGuid ChannelGUID;
};

USTRUCT(BlueprintType)
struct FNetConfigInfo
{
	GENERATED_BODY()

	FNetConfigInfo()
		: Version(TEXT("1.0.1"))
		, URL(TEXT("127.0.0.1"))
		, Port(11223)
		, RecvDataSize(BUFFER_SIZE)
		, SendDataSize(BUFFER_SIZE)
		, MaxConnections(CONNECT_NUM)
		, MaxChannels(CHANNEL_NUM)
	{}

	UPROPERTY()
	FString Version;

	UPROPERTY()
	FString URL;

	UPROPERTY()
	int32 Port;

	UPROPERTY()
	int32 RecvDataSize;

	UPROPERTY()
	int32 SendDataSize;

	UPROPERTY()
	int32 MaxConnections;

	UPROPERTY()
	int32 MaxChannels;
};