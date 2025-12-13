#include "NetChannelIOStream.h"


FNetChannelIOStream& FNetChannelIOStream::operator<<(FString& InValue)
{
	ANSICHAR* Str = TCHAR_TO_UTF8(*InValue);
	Write(Str, (InValue.Len() + 1) * sizeof(ANSICHAR));
	return *this;
}

FNetChannelIOStream& FNetChannelIOStream::operator>>(FString& InValue)
{
	InValue = UTF8_TO_TCHAR(StreamPtr);
	Seek(InValue.Len() + 1);
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


