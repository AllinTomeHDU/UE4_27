#pragma once

#include "CoreMinimal.h"


#if PLATFORM_WINDOWS
#pragma optimize("",off)
#endif
struct FNetChannelEncryption
{
	static constexpr uint16 MagicCode = 0xBABA; // 数据包固定签名

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

	static bool EncryptForSend(const TArray<uint8>& InPlainData, TArray<uint8>& OutEncryptedPacket)
	{
		if (InPlainData.Num() == 0) return false;

		const uint32 RandomKey = FMath::Rand();

		TArray<uint8> EncryptedData = InPlainData;
		XorEncryptWithKey(EncryptedData, RandomKey);

		const int32 FinalSize = sizeof(uint16) + sizeof(uint32) + EncryptedData.Num();
		OutEncryptedPacket.SetNumUninitialized(FinalSize);

		uint8* WritePtr = OutEncryptedPacket.GetData();

		FMemory::Memcpy(WritePtr, &MagicCode, sizeof(uint16));
		WritePtr += sizeof(uint16);

		FMemory::Memcpy(WritePtr, &RandomKey, sizeof(uint32));
		WritePtr += sizeof(uint32);

		FMemory::Memcpy(WritePtr, EncryptedData.GetData(), EncryptedData.Num());
		return true;
	}

	static bool DecryptForRecv(const TArray<uint8>& InEncryptedPacket, TArray<uint8>& OutPlainData)
	{
		if (InEncryptedPacket.Num() <= sizeof(uint16) + sizeof(uint32)) return false;

		const uint8* ReadPtr = InEncryptedPacket.GetData();

		uint16 Magic = 0;
		FMemory::Memcpy(&Magic, ReadPtr, sizeof(uint16));
		ReadPtr += sizeof(uint16);
		if (Magic != MagicCode)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Encryptor] invalid Magic=0x%04X"), Magic);
			return false;
		}

		uint32 RandomKey = 0;
		FMemory::Memcpy(&RandomKey, ReadPtr, sizeof(uint32));
		ReadPtr += sizeof(uint32);

		const int32 PayloadSize = InEncryptedPacket.Num() - sizeof(uint16) - sizeof(uint32);
		OutPlainData.SetNumUninitialized(PayloadSize);
		FMemory::Memcpy(OutPlainData.GetData(), ReadPtr, PayloadSize);

		XorEncryptWithKey(OutPlainData, RandomKey);
		return true;
	}
};
#if PLATFORM_WINDOWS
#pragma optimize("",on)
#endif
