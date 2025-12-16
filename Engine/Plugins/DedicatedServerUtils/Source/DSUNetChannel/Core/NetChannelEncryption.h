#pragma once

#include "CoreMinimal.h"


#if PLATFORM_WINDOWS
#pragma optimize("",off)
#endif
struct FNetChannelEncryption
{
	static constexpr uint16 MagicCode = 0xDEBA; // 数据包固定签名

	static void XorEncryptWithKey(TArray<uint8>& Buffer, uint32 Key)
	{
		const uint64 ExtendedKey = (uint64)Key | ((uint64)Key << 32);

		const int32 DataSize = Buffer.Num();
		uint8* DataPtr = Buffer.GetData();

		const int32 BlockCount = DataSize / 8;
		const int32 Remaining = DataSize % 8;

		uint64* Data64 = reinterpret_cast<uint64*>(DataPtr);
		for (int32 i = 0; i < BlockCount; ++i)
		{
			Data64[i] ^= ExtendedKey;
		}

		const uint8* KeyBytes = reinterpret_cast<const uint8*>(&Key);
		uint8* Tail = DataPtr + BlockCount * 8;
		for (int32 i = 0; i < Remaining; ++i)
		{
			Tail[i] ^= KeyBytes[i % 4];
		}
	}

	static bool EncryptForSend(TArray<uint8>& Data)
	{
		if (Data.Num() == 0) return false;

		const uint32 RandomKey = FMath::Rand();

		XorEncryptWithKey(Data, RandomKey);

		const int32 HeaderSize = sizeof(uint16) + sizeof(uint32);
		const int32 FinalSize = HeaderSize + Data.Num();
		Data.InsertUninitialized(0, HeaderSize);

		uint8* WritePtr = Data.GetData();

		FMemory::Memcpy(WritePtr, &MagicCode, sizeof(uint16));
		WritePtr += sizeof(uint16);

		FMemory::Memcpy(WritePtr, &RandomKey, sizeof(uint32));
		return true;
	}

	static bool DecryptForRecv(TArray<uint8>& Data)
	{
		const int32 HeaderSize = sizeof(uint16) + sizeof(uint32);
		if (Data.Num() <= HeaderSize) return false;

		uint8* DataPtr = Data.GetData();

		uint16 Magic = 0;
		FMemory::Memcpy(&Magic, DataPtr, sizeof(uint16));
		if (Magic != MagicCode)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Encryptor] invalid Magic=0x%04X"), Magic);
			return false;
		}

		uint32 RandomKey = 0;
		FMemory::Memcpy(&RandomKey, DataPtr + sizeof(uint16), sizeof(uint32));

		Data.RemoveAt(0, HeaderSize, false);

		XorEncryptWithKey(Data, RandomKey);
		return true;
	}
};
#if PLATFORM_WINDOWS
#pragma optimize("",on)
#endif
