#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "NetChannelType.generated.h"

#define CHANNEL_NUM 5
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
	Verify,
	Join
};

struct DS_NETCHANNEL_API FNetBunchHead
{
	uint32 ProtocolsNumber;
	FGuid ChannelGUID;
};

USTRUCT(BlueprintType)
struct DS_NETCHANNEL_API FNetConfigInfo
{
	GENERATED_BODY()

	FNetConfigInfo()
		: Version(TEXT("1.0.1"))
		, URL(TEXT("127.0.0.1"))
		, Port(10099)
		, RecvDataSize(BUFFER_SIZE)
		, SendDataSize(BUFFER_SIZE)
		, MaxConnections(CONNECT_NUM)
		, MaxChannels(CHANNEL_NUM)
		, HeartBeatTimeInterval(5.f)
		, HeatBeatTimeOutTime(20.f)
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

	UPROPERTY()
	float HeartBeatTimeInterval;

	UPROPERTY()
	float HeatBeatTimeOutTime;
};

struct DS_NETCHANNEL_API FNetAddr
{
	FNetAddr() : IP(0), Port(0) {}

	FNetAddr(const uint32& InIP, const uint32& InPort)
		: IP(InIP), Port(InPort)
	{}

	FNetAddr(const FString& InIpStr, const uint32& InPort)
		: IP(0), Port(InPort)
	{
		TArray<FString> Parts;
		InIpStr.ParseIntoArray(Parts, TEXT("."), true);
		if (Parts.Num() == 4)
		{
			uint8 A = FCString::Atoi(*Parts[0]);
			uint8 B = FCString::Atoi(*Parts[1]);
			uint8 C = FCString::Atoi(*Parts[2]);
			uint8 D = FCString::Atoi(*Parts[3]);

			FIPv4Address IPv4(A, B, C, D);
			IP = IPv4.Value;
		}
	}

	uint32 IP;
	uint32 Port;
};

struct DS_NETCHANNEL_API FNetChannelAddrInfo
{
	FNetChannelAddrInfo() {}

	FNetChannelAddrInfo(const FNetAddr& InAddr, const FGuid& InGUID)
		: Addr(InAddr), GUID(InGUID)
	{}

	FNetAddr Addr;
	FGuid GUID;
};

struct DS_NETCHANNEL_API FNetServerInfo
{
	FNetServerInfo() : ID(0), Addr(FNetAddr()) 
	{
		FMemory::Memset(Name, 0, sizeof(Name));
	}

	uint32 ID;
	char Name[20];
	FNetAddr Addr;
};
