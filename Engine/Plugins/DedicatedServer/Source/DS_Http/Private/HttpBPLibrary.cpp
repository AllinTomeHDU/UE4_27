#include "HttpBPLibrary.h"
#include "HttpManagement.h"


FName UHttpBPLibrary::GetHandleByLastExecutionRequest()
{
	return HTTP_INSTANCE.GetHandleByLastExecutionRequest();
}

void UHttpBPLibrary::Pause()
{
	HTTP_INSTANCE.Suspend();
}

void UHttpBPLibrary::Awaken()
{
	HTTP_INSTANCE.Awake();
}

bool UHttpBPLibrary::Cancel()
{
	return HTTP_INSTANCE.Cancel();
}

bool UHttpBPLibrary::CancelByHandle(const FName& Handle)
{
	return HTTP_INSTANCE.Cancel(Handle);
}

bool UHttpBPLibrary::PostRequest(const FString& InURL, const FString& InParam, const FHttpResponseDelegateBP& ResponseDelegateBP)
{
	return HTTP_INSTANCE.PostRequest(*InURL, *InParam, ResponseDelegateBP);
}

bool UHttpBPLibrary::GetObjectToMemory(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL)
{
	return HTTP_INSTANCE.GetObjectToMemory(ResponseDelegateBP, URL);
}

void UHttpBPLibrary::GetObjectsToMemory(const FHttpResponseDelegateBP& ResponseDelegateBP, const TArray<FString>& URL)
{
	HTTP_INSTANCE.GetObjectsToMemory(ResponseDelegateBP, URL);
}

bool UHttpBPLibrary::GetObjectToLocal(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const FString& SavePaths)
{
	return HTTP_INSTANCE.GetObjectToLocal(ResponseDelegateBP, URL, SavePaths);
}

void UHttpBPLibrary::GetObjectsToLocal(const FHttpResponseDelegateBP& ResponseDelegateBP, const TArray<FString>& URL, const FString& SavePaths)
{
	HTTP_INSTANCE.GetObjectsToLocal(ResponseDelegateBP, URL, SavePaths);
}

bool UHttpBPLibrary::PutObjectFromLocal(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const FString& LocalPaths)
{
	return HTTP_INSTANCE.PutObjectFromLocal(ResponseDelegateBP, URL, LocalPaths);
}

bool UHttpBPLibrary::PutObjectsFromLocal(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const FString& LocalPaths)
{
	return HTTP_INSTANCE.PutObjectsFromLocal(ResponseDelegateBP, URL, LocalPaths);
}

bool UHttpBPLibrary::PutObjectFromBuffer(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const TArray<uint8>& Buffer)
{
	return HTTP_INSTANCE.PutObjectFromBuffer(ResponseDelegateBP, URL, Buffer);
}

bool UHttpBPLibrary::PutObjectFromString(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const FString& InString)
{
	return HTTP_INSTANCE.PutObjectFromString(ResponseDelegateBP, URL, InString);
}

bool UHttpBPLibrary::DeleteObject(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL)
{
	return HTTP_INSTANCE.DeleteObject(ResponseDelegateBP, URL);
}

void UHttpBPLibrary::DeleteObjects(const FHttpResponseDelegateBP& ResponseDelegateBP, const TArray<FString>& URL)
{
	HTTP_INSTANCE.DeleteObjects(ResponseDelegateBP, URL);
}

void UHttpBPLibrary::Tick(float DeltaTime)
{
	FHttpManagement::Get()->Tick(DeltaTime);
}

