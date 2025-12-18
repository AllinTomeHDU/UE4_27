#pragma once

#include "CoreMinimal.h"


class DS_NETCHANNEL_API FNetChannelIOStream
{
public:
	TArray<uint8>& Buffer;

public:
	FNetChannelIOStream(TArray<uint8>& InBuffer) 
		: Buffer(InBuffer), StreamPtr(InBuffer.GetData())
	{}

	template<class T>
	FNetChannelIOStream& operator<<(T& InValue)
	{
		Write((void*)&InValue, sizeof(T));
		return *this;
	}
	template<class T>
	FNetChannelIOStream& operator<<(TArray<T>& InValue)
	{
		FNetChannelIOStream& Stream = *this;
		int32 ArrayNum = InValue.Num();
		Stream << ArrayNum;
		for (T& Tmp : InValue)
		{
			Stream << Tmp;
		}
		return *this;
	}
	FNetChannelIOStream& operator<<(FString& InValue);

	template<class T>
	FNetChannelIOStream& operator>>(T& InValue)
	{
		Read<T>(InValue);
		return *this;
	}
	template<class T>
	FNetChannelIOStream& operator>>(TArray<T>& InValue)
	{
		FNetChannelIOStream& Stream = *this;
		int32 ArrayNum = 0;
		Stream >> ArrayNum;
		for (int i = 0; i < ArrayNum; ++i)
		{
			InValue.Add(T());
			T& LastValue = InValue.Last();
			Stream >> LastValue;
		}
		return *this;
	}
	FNetChannelIOStream& operator>>(FString& InValue);

	void Seek(int32 InPos = 1);

protected:
	void Write(void* InData, int64 InLength);

	template<class T>
	void Read(T& InValue)
	{
		Seek(0);  // 确保 StreamPtr 不为空
		InValue = *(T*)StreamPtr;
		Seek(sizeof(T));
	}

private:
	uint8* StreamPtr;

public:
	FORCEINLINE uint8* Begin() const { return Buffer.GetData(); }
	FORCEINLINE uint8* End() const { return Buffer.GetData() + Buffer.Num(); }
	FORCEINLINE uint8* Ptr() const { return StreamPtr; }
};

