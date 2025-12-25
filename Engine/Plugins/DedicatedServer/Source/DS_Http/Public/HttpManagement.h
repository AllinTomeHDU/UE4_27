#pragma once

#include "CoreMinimal.h"
#include "Definition/HttpType.h"

#ifdef PLATFORM_PROJECT
#include "Tickable.h"
#endif

struct FHttpActionRequest;

const FName NONE_NAME = TEXT("NONE");
typedef FName FHttpHandle;

class DS_HTTP_API FHttpManagement
#ifdef PLATFORM_PROJECT
	:public FTickableGameObject
#endif
{
	struct DS_HTTP_API FHTTP
	{
		friend class FHttpManagement;

		FHTTP() : bPause(false) {}

		FORCEINLINE FHttpHandle GetHandleByLastExecutionRequest() { return TmpStorageHandle; }
		FORCEINLINE void Awake() { bPause = false; }
		FORCEINLINE void Suspend() { bPause = true; }
		bool Suspend(const FHttpHandle& Handle);
		bool Cancel();
		bool Cancel(const FHttpHandle& Handle);

		bool PostRequest(const TCHAR* URL, const TCHAR* Param, const FHttpResponseDelegate& ResponseDelegate);
		bool PostRequest(const TCHAR* URL, const TCHAR* Param, const FHttpResponseDelegateBP& ResponseDelegateBP);

		bool GetObjectToMemory(const FHttpResponseDelegate& ResponseDelegate, const FString& URL);
		bool GetObjectToMemory(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL);

		void GetObjectsToMemory(const FHttpResponseDelegate& ResponseDelegate, const TArray<FString>& URL);
		void GetObjectsToMemory(const FHttpResponseDelegateBP& ResponseDelegateBP, const TArray<FString>& URL);

		bool GetObjectToLocal(const FHttpResponseDelegate& ResponseDelegate, const FString& URL, const FString& SavePaths);
		bool GetObjectToLocal(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const FString& SavePaths);

		void GetObjectsToLocal(const FHttpResponseDelegate& ResponseDelegate, const TArray<FString>& URL, const FString& SavePaths);
		void GetObjectsToLocal(const FHttpResponseDelegateBP& ResponseDelegateBP, const TArray<FString>& URL, const FString& SavePaths);

		bool PutObjectFromLocal(const FHttpResponseDelegate& ResponseDelegate, const FString& URL, const FString& LocalPaths);
		bool PutObjectFromLocal(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const FString& LocalPaths);

		bool PutObjectsFromLocal(const FHttpResponseDelegate& ResponseDelegate, const FString& URL, const FString& LocalPaths);
		bool PutObjectsFromLocal(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const FString& LocalPaths);

		bool PutObjectFromBuffer(const FHttpResponseDelegate& ResponseDelegate, const FString& URL, const TArray<uint8>& Buffer);
		bool PutObjectFromBuffer(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const TArray<uint8>& Buffer);

		bool PutObjectFromString(const FHttpResponseDelegate& ResponseDelegate, const FString& URL, const FString& InString);
		bool PutObjectFromString(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const FString& InString);

		bool PutObjectFromStream(const FHttpResponseDelegate& ResponseDelegate, const FString& URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream);
		bool PutObjectFromStream(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream);

		bool DeleteObject(const FHttpResponseDelegate& ResponseDelegate, const FString& URL);
		bool DeleteObject(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL);

		void DeleteObjects(const FHttpResponseDelegate& ResponseDelegate, const TArray<FString>& URL);
		void DeleteObjects(const FHttpResponseDelegateBP& ResponseDelegateBP, const TArray<FString>& URL);

	private:
		FHttpHandle RegisteredHttpRequest(EHttpRequestType RequestType = EHttpRequestType::SINGLE,
			FOnCompleteDelegate RequestCompleteDelegate = nullptr,
			FOnRequestProgressDelegate RequestProgressDelegate = nullptr,
			FOnRequestHeaderReceivedDelegate RequestHeaderReceivedDelegate = nullptr,
			FSimpleDelegate AllRequestCompleteDelegate = nullptr
		);
		FHttpHandle RegisteredHttpRequest(EHttpRequestType RequestType = EHttpRequestType::SINGLE,
			FOnRequestCompleteDelegateBP RequestCompleteDelegate = FOnRequestCompleteDelegateBP(),
			FOnRequestProgressDelegateBP RequestProgressDelegate = FOnRequestProgressDelegateBP(),
			FOnRequestHeaderReceivedDelegateBP RequestHeaderReceivedDelegate = FOnRequestHeaderReceivedDelegateBP(),
			FOnAllRequestsCompleteDelegateBP AllRequestCompleteDelegate = FOnAllRequestsCompleteDelegateBP()
		);
		
		TWeakPtr<FHttpActionRequest> Find(const FHttpHandle& Handle);

		bool PostRequest(const FHttpHandle& Handle, const FString& URL);
		bool GetObjectToMemory(const FHttpHandle& Handle, const FString& URL);
		void GetObjectsToMemory(const FHttpHandle& Handle, const TArray<FString>& URL);
		bool GetObjectToLocal(const FHttpHandle& Handle, const FString& URL, const FString& SavePaths);
		void GetObjectsToLocal(const FHttpHandle& Handle, const TArray<FString>& URL, const FString& SavePaths);
		bool PutObjectFromLocal(const FHttpHandle& Handle, const FString& URL, const FString& LocalPaths);
		bool PutObjectsFromLocal(const FHttpHandle& Handle, const FString& URL, const FString& LocalPaths);
		bool PutObjectFromBuffer(const FHttpHandle& Handle, const FString& URL, const TArray<uint8>& Buffer);
		bool PutObjectFromString(const FHttpHandle& Handle, const FString& URL, const FString& InString);
		bool PutObjectFromStream(const FHttpHandle& Handle, const FString& URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream);
		bool DeleteObject(const FHttpHandle& Handle, const FString& URL);
		void DeleteObjects(const FHttpHandle& Handle, const TArray<FString>& URL);

	private:
		FName TmpStorageHandle;
		bool bPause;
		TMap<FHttpHandle, TSharedPtr<FHttpActionRequest>> HttpMap;
	};

public:
	virtual ~FHttpManagement() {};
	static FHttpManagement* Get();
	FORCEINLINE FHTTP& GetHTTP() { return HTTP; }
	virtual void Tick(float DeltaTime);

protected:
	virtual TStatId GetStatId() const { return TStatId(); }
	virtual bool IsTickableInEditor() const { return true; }

private:
	static FHttpManagement* Instance;
	FHTTP HTTP;
	FCriticalSection Mutex;
};

#define HTTP_INSTANCE FHttpManagement::Get()->GetHTTP()