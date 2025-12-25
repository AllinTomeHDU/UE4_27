#include "HttpManagement.h"
#include "Definition/HttpMacro.h"
#include "Request/HttpActionSingleRequest.h"
#include "Request/HttpActionMultipleRequest.h"
#include "HttpModule.h"
#include "HttpManager.h"


FHttpManagement* FHttpManagement::Instance = nullptr;
FHttpManagement* FHttpManagement::Get()
{
	if (!Instance)
	{
		Instance = new FHttpManagement();
	}
	return Instance;
}

void FHttpManagement::Tick(float DeltaTime)
{
	FScopeLock ScopeLock(&Instance->Mutex);

	if (!HTTP.bPause)
	{
		FHttpModule::Get().GetHttpManager().Tick(DeltaTime);
	}

	TArray<FName> RemoveRequest;
	for (auto& Tmp : HTTP.HttpMap)
	{
		if (Tmp.Value->IsRequestComplete())
		{
			RemoveRequest.Add(Tmp.Key);
		}
	}
	for (auto& Tmp : RemoveRequest)
	{
		GetHTTP().HttpMap.Remove(Tmp);
	}
}

TSharedPtr<FHttpActionRequest> GetHttpActionRequest(EHttpRequestType RequestType)
{
	TSharedPtr<FHttpActionRequest> HttpObject = nullptr;
	switch (RequestType)
	{
		case EHttpRequestType::SINGLE:
		{
			HttpObject = MakeShareable(new FHttpActionSingleRequest());
			break;
		}
		case EHttpRequestType::MULTPLE:
		{
			HttpObject = MakeShareable(new FHttpActionMultipleRequest());
			break;
		}
	}
	return HttpObject;
}

bool FHttpManagement::FHTTP::Suspend(const FHttpHandle& Handle)
{
	TWeakPtr<FHttpActionRequest> Object = Find(Handle);
	return Object.IsValid() ? Object.Pin()->Suspend() : false;
}

bool FHttpManagement::FHTTP::Cancel()
{
	bool bCancel = true;
	for (auto& Tmp : Instance->GetHTTP().HttpMap)
	{
		if (!Tmp.Value->Cancel())
		{
			bCancel = false;
			break;
		}
	}
	return bCancel;
}

bool FHttpManagement::FHTTP::Cancel(const FHttpHandle& Handle)
{
	TWeakPtr<FHttpActionRequest> Object = Find(Handle);
	return Object.IsValid() ? Object.Pin()->Cancel() : false;
}

bool FHttpManagement::FHTTP::PostRequest(const FHttpHandle& Handle, const FString& URL)
{
	TWeakPtr<FHttpActionRequest> Object = Find(Handle);
	return Object.IsValid() ? Object.Pin()->PostObject(URL) : false;
}

bool FHttpManagement::FHTTP::PostRequest(const TCHAR* URL, const TCHAR* Param, const FHttpResponseDelegate& ResponseDelegate)
{
	HTTP_REGISTERED_REQUEST(EHttpRequestType::SINGLE);
	return PostRequest(Handle, URL + FString(TEXT("?")) + Param);
}

bool FHttpManagement::FHTTP::PostRequest(const TCHAR* URL, const TCHAR* Param, const FHttpResponseDelegateBP& ResponseDelegateBP)
{
	HTTP_REGISTERED_REQUEST_BP(EHttpRequestType::SINGLE);
	return PostRequest(Handle, URL + FString(TEXT("?")) + Param);
}

bool FHttpManagement::FHTTP::GetObjectToMemory(const FHttpHandle& Handle, const FString& URL)
{
	TWeakPtr<FHttpActionRequest> Object = Find(Handle);
	return Object.IsValid() ? Object.Pin()->GetObject(URL) : false;
}

bool FHttpManagement::FHTTP::GetObjectToMemory(const FHttpResponseDelegate& ResponseDelegate, const FString& URL)
{
	HTTP_REGISTERED_REQUEST(EHttpRequestType::SINGLE);
	return GetObjectToMemory(Handle, URL);
}

bool FHttpManagement::FHTTP::GetObjectToMemory(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL)
{
	HTTP_REGISTERED_REQUEST_BP(EHttpRequestType::SINGLE);
	return GetObjectToMemory(Handle, URL);
}

void FHttpManagement::FHTTP::GetObjectsToMemory(const FHttpHandle& Handle, const TArray<FString>& URL)
{
	TWeakPtr<FHttpActionRequest> Object = Find(Handle);
	if (Object.IsValid())
	{
		Object.Pin()->GetObjects(URL);
	}
}

void FHttpManagement::FHTTP::GetObjectsToMemory(const FHttpResponseDelegate& ResponseDelegate, const TArray<FString>& URL)
{
	HTTP_REGISTERED_REQUEST(EHttpRequestType::MULTPLE);
	GetObjectsToMemory(Handle, URL);
}

void FHttpManagement::FHTTP::GetObjectsToMemory(const FHttpResponseDelegateBP& ResponseDelegateBP, const TArray<FString>& URL)
{
	HTTP_REGISTERED_REQUEST_BP(EHttpRequestType::MULTPLE);
	GetObjectsToMemory(Handle, URL);
}

bool FHttpManagement::FHTTP::GetObjectToLocal(const FHttpHandle& Handle, const FString& URL, const FString& SavePaths)
{
	TWeakPtr<FHttpActionRequest> Object = Find(Handle);
	return Object.IsValid() ? Object.Pin()->GetObject(URL, SavePaths) : false;
}

bool FHttpManagement::FHTTP::GetObjectToLocal(const FHttpResponseDelegate& ResponseDelegate, const FString& URL, const FString& SavePaths)
{
	HTTP_REGISTERED_REQUEST(EHttpRequestType::SINGLE);
	return GetObjectToLocal(Handle, URL, SavePaths);
}

bool FHttpManagement::FHTTP::GetObjectToLocal(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const FString& SavePaths)
{
	HTTP_REGISTERED_REQUEST_BP(EHttpRequestType::SINGLE);
	return GetObjectToLocal(Handle, URL, SavePaths);
}

void FHttpManagement::FHTTP::GetObjectsToLocal(const FHttpHandle& Handle, const TArray<FString>& URL, const FString& SavePaths)
{
	TWeakPtr<FHttpActionRequest> Object = Find(Handle);
	if (Object.IsValid())
	{
		Object.Pin()->GetObjects(URL, SavePaths);
	}
}

void FHttpManagement::FHTTP::GetObjectsToLocal(const FHttpResponseDelegate& ResponseDelegate, const TArray<FString>& URL, const FString& SavePaths)
{
	HTTP_REGISTERED_REQUEST(EHttpRequestType::MULTPLE);
	GetObjectsToLocal(Handle, URL, SavePaths);
}

void FHttpManagement::FHTTP::GetObjectsToLocal(const FHttpResponseDelegateBP& ResponseDelegateBP, const TArray<FString>& URL, const FString& SavePaths)
{
	HTTP_REGISTERED_REQUEST_BP(EHttpRequestType::MULTPLE);
	GetObjectsToLocal(Handle, URL, SavePaths);
}

bool FHttpManagement::FHTTP::PutObjectFromLocal(const FHttpHandle& Handle, const FString& URL, const FString& LocalPaths)
{
	TWeakPtr<FHttpActionRequest> Object = Find(Handle);
	return Object.IsValid() ? Object.Pin()->PutObject(URL, LocalPaths) : false;
}

bool FHttpManagement::FHTTP::PutObjectFromLocal(const FHttpResponseDelegate& ResponseDelegate, const FString& URL, const FString& LocalPaths)
{
	HTTP_REGISTERED_REQUEST(EHttpRequestType::SINGLE);
	return PutObjectFromLocal(Handle, URL, LocalPaths);
}

bool FHttpManagement::FHTTP::PutObjectFromLocal(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const FString& LocalPaths)
{
	HTTP_REGISTERED_REQUEST_BP(EHttpRequestType::SINGLE);
	return PutObjectFromLocal(Handle, URL, LocalPaths);
}

bool FHttpManagement::FHTTP::PutObjectsFromLocal(const FHttpHandle& Handle, const FString& URL, const FString& LocalPaths)
{
	return PutObjectFromLocal(Handle, URL, LocalPaths);
}

bool FHttpManagement::FHTTP::PutObjectsFromLocal(const FHttpResponseDelegate& ResponseDelegate, const FString& URL, const FString& LocalPaths)
{
	HTTP_REGISTERED_REQUEST(EHttpRequestType::MULTPLE);
	return PutObjectsFromLocal(Handle, URL, LocalPaths);
}

bool FHttpManagement::FHTTP::PutObjectsFromLocal(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const FString& LocalPaths)
{
	HTTP_REGISTERED_REQUEST_BP(EHttpRequestType::MULTPLE);
	return PutObjectsFromLocal(Handle, URL, LocalPaths);
}

bool FHttpManagement::FHTTP::PutObjectFromBuffer(const FHttpHandle& Handle, const FString& URL, const TArray<uint8>& Buffer)
{
	TWeakPtr<FHttpActionRequest> Object = Find(Handle);
	return Object.IsValid() ? Object.Pin()->PutObject(URL, Buffer) : false;
}

bool FHttpManagement::FHTTP::PutObjectFromBuffer(const FHttpResponseDelegate& ResponseDelegate, const FString& URL, const TArray<uint8>& Buffer)
{
	HTTP_REGISTERED_REQUEST(EHttpRequestType::SINGLE);
	return PutObjectFromBuffer(Handle, URL, Buffer);
}

bool FHttpManagement::FHTTP::PutObjectFromBuffer(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const TArray<uint8>& Buffer)
{
	HTTP_REGISTERED_REQUEST_BP(EHttpRequestType::SINGLE);
	return PutObjectFromBuffer(Handle, URL, Buffer);
}

bool FHttpManagement::FHTTP::PutObjectFromString(const FHttpHandle& Handle, const FString& URL, const FString& InString)
{
	TWeakPtr<FHttpActionRequest> Object = Find(Handle);
	return Object.IsValid() ? Object.Pin()->PutObjectByString(URL, InString) : false;
}

bool FHttpManagement::FHTTP::PutObjectFromString(const FHttpResponseDelegate& ResponseDelegate, const FString& URL, const FString& InString)
{
	HTTP_REGISTERED_REQUEST(EHttpRequestType::SINGLE);
	return PutObjectFromString(Handle, URL, InString);
}

bool FHttpManagement::FHTTP::PutObjectFromString(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const FString& InString)
{
	HTTP_REGISTERED_REQUEST_BP(EHttpRequestType::SINGLE);
	return PutObjectFromString(Handle, URL, InString);
}

bool FHttpManagement::FHTTP::PutObjectFromStream(const FHttpHandle& Handle, const FString& URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream)
{
	TWeakPtr<FHttpActionRequest> Object = Find(Handle);
	return Object.IsValid() ? Object.Pin()->PutObject(URL, Stream) : false;
}

bool FHttpManagement::FHTTP::PutObjectFromStream(const FHttpResponseDelegate& ResponseDelegate, const FString& URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream)
{
	HTTP_REGISTERED_REQUEST(EHttpRequestType::SINGLE);
	return PutObjectFromStream(Handle, URL, Stream);
}

bool FHttpManagement::FHTTP::PutObjectFromStream(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream)
{
	HTTP_REGISTERED_REQUEST_BP(EHttpRequestType::SINGLE);
	return PutObjectFromStream(Handle, URL, Stream);
}

bool FHttpManagement::FHTTP::DeleteObject(const FHttpHandle& Handle, const FString& URL)
{
	TWeakPtr<FHttpActionRequest> Object = Find(Handle);
	return Object.IsValid() ? Object.Pin()->DeleteObject(URL) : false;
}

bool FHttpManagement::FHTTP::DeleteObject(const FHttpResponseDelegate& ResponseDelegate, const FString& URL)
{
	HTTP_REGISTERED_REQUEST(EHttpRequestType::SINGLE);
	return DeleteObject(Handle, URL);
}

bool FHttpManagement::FHTTP::DeleteObject(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL)
{
	HTTP_REGISTERED_REQUEST_BP(EHttpRequestType::SINGLE);
	return DeleteObject(Handle, URL);
}

void FHttpManagement::FHTTP::DeleteObjects(const FHttpHandle& Handle, const TArray<FString>& URL)
{
	TWeakPtr<FHttpActionRequest> Object = Find(Handle);
	if (Object.IsValid())
	{
		Object.Pin()->DeleteObjects(URL);
	}
}

void FHttpManagement::FHTTP::DeleteObjects(const FHttpResponseDelegate& ResponseDelegate, const TArray<FString>& URL)
{
	HTTP_REGISTERED_REQUEST(EHttpRequestType::MULTPLE);
	DeleteObjects(Handle, URL);
}

void FHttpManagement::FHTTP::DeleteObjects(const FHttpResponseDelegateBP& ResponseDelegateBP, const TArray<FString>& URL)
{
	HTTP_REGISTERED_REQUEST_BP(EHttpRequestType::MULTPLE);
	DeleteObjects(Handle, URL);
}

FHttpHandle FHttpManagement::FHTTP::RegisteredHttpRequest(EHttpRequestType RequestType, FOnCompleteDelegate RequestCompleteDelegate, FOnRequestProgressDelegate RequestProgressDelegate,
														  FOnRequestHeaderReceivedDelegate RequestHeaderReceivedDelegate, FSimpleDelegate AllRequestCompleteDelegate)
{
	FScopeLock ScopeLock(&Instance->Mutex);

	TSharedPtr<FHttpActionRequest> HttpObject = GetHttpActionRequest(RequestType);
	HttpObject->OnCompleteDelegate = RequestCompleteDelegate;
	HttpObject->OnRequestProgressDelegate = RequestProgressDelegate;
	HttpObject->OnRequestHeaderReceivedDelegate = RequestHeaderReceivedDelegate;
	HttpObject->OnAllTasksCompletedDelegate = AllRequestCompleteDelegate;

	FHttpHandle Key = *FGuid::NewGuid().ToString();
	HttpMap.Add(Key, HttpObject);
	return Key;
}

FHttpHandle FHttpManagement::FHTTP::RegisteredHttpRequest(EHttpRequestType RequestType, FOnRequestCompleteDelegateBP RequestCompleteDelegate, FOnRequestProgressDelegateBP RequestProgressDelegate, 
														  FOnRequestHeaderReceivedDelegateBP RequestHeaderReceivedDelegate, FOnAllRequestsCompleteDelegateBP AllRequestCompleteDelegate)
{
	FScopeLock ScopeLock(&Instance->Mutex);

	TSharedPtr<FHttpActionRequest> HttpObject = GetHttpActionRequest(RequestType);
	HttpObject->OnHttpRequestCompleteDelegateBP = RequestCompleteDelegate;
	HttpObject->OnHttpRequestProgressDelegateBP = RequestProgressDelegate;
	HttpObject->OnHttpRequestHeaderReceivedDelegateBP = RequestHeaderReceivedDelegate;
	HttpObject->OnAllRequestCompleteDelegateBP = AllRequestCompleteDelegate;

	FHttpHandle Key = *FGuid::NewGuid().ToString();
	HttpMap.Add(Key, HttpObject);
	return Key;
}

TWeakPtr<FHttpActionRequest> FHttpManagement::FHTTP::Find(const FHttpHandle& Handle)
{
	FScopeLock ScopeLock(&Instance->Mutex);

	TWeakPtr<FHttpActionRequest> Object = nullptr;
	for (auto& Tmp : Instance->GetHTTP().HttpMap)
	{
		if (Tmp.Key == Handle)
		{
			Object = Tmp.Value;
			break;
		}
	}
	return Object;
}
