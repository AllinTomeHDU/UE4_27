#pragma once

#include "CoreMinimal.h"


#if PLATFORM_WINDOWS
#pragma optimize("",off)
#endif
struct FNetChannelEncryption
{
	static void XorEncryptWithKey(TArray<uint8>& Buffer, uint32 XorKey = 0xABCD2025)
	{
		const uint64 ExtendedKey = (uint64)XorKey | ((uint64)XorKey << 32);

		const int32 DataSize = Buffer.Num();
		uint8* DataPtr = Buffer.GetData();

		const int32 BlockCount = DataSize / 8;
		const int32 Remaining = DataSize % 8;

		uint64* Data64 = reinterpret_cast<uint64*>(DataPtr);
		for (int32 i = 0; i < BlockCount; ++i)
		{
			Data64[i] ^= ExtendedKey;
		}

		const uint8* KeyBytes = reinterpret_cast<const uint8*>(&XorKey);
		uint8* Tail = DataPtr + BlockCount * 8;
		for (int32 i = 0; i < Remaining; ++i)
		{
			Tail[i] ^= KeyBytes[i % 4];
		}
	}
};
#if PLATFORM_WINDOWS
#pragma optimize("",on)
#endif
