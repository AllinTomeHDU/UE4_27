#include "NetChannelIOStream.h"


FNetChannelIOStream& FNetChannelIOStream::operator<<(FString& InValue)
{
	*this << InValue.GetCharArray();
	return *this;
}

FNetChannelIOStream& FNetChannelIOStream::operator>>(FString& InValue)
{
	*this >> InValue.GetCharArray();
	return *this;
}

void FNetChannelIOStream::Seek(int32 InPos)
{
	StreamPtr = (!StreamPtr ? Buffer.GetData() : StreamPtr) + InPos;
}

void FNetChannelIOStream::Write(void* InData, int64 InLength)
{
	int32 StartPos = Buffer.AddUninitialized(InLength);
	FMemory::Memcpy(&Buffer[StartPos], InData, InLength);
}


