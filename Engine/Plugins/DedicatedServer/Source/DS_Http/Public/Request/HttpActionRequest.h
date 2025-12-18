#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "../Core/HttpType.h"


struct DS_HTTP_API FHttpActionRequest : public TSharedFromThis<FHttpActionRequest>
{
public:
	// C++
	FOnCompleteDelegate					OnCompleteDelegate;
	FOnRequestProgressDelegate			OnRequestProgressDelegate;
	FOnRequestHeaderReceivedDelegate	OnRequestHeaderReceivedDelegate;
	FSimpleDelegate						OnAllTasksCompletedDelegate;

	// BP
	FOnRequestCompleteDelegateBP		OnHttpRequestCompleteDelegateBP;
	FOnRequestProgressDelegateBP		OnHttpRequestProgressDelegateBP;
	FOnRequestHeaderReceivedDelegateBP	OnHttpRequestHeaderReceivedDelegateBP;
	FOnAllRequestsCompleteDelegateBP	OnAllRequestCompleteDelegateBP;

	FHttpActionRequest() : bRequestComplete(false), bSaveDisk(true) {}
	virtual ~FHttpActionRequest() {}

	virtual bool Suspend() { return false; }
	virtual bool Cancel() { return false; }

	virtual void GetObjects(const TArray<FString>& URL, const FString& SavePaths = TEXT("")) {};
	virtual void DeleteObjects(const TArray<FString>& URL) {};

	virtual bool GetObject(const FString& URL, const FString& SavePaths = TEXT("")) { return false; };
	virtual bool PutObject(const FString& URL, const TArray<uint8>& Data) { return false; };
	virtual bool PutObject(const FString& URL, const FString& LocalPaths) { return false; };
	virtual bool PutObject(const FString& URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream) { return false; };
	virtual bool PutObjectByString(const FString& URL, const FString& InBuff) { return false; };
	virtual bool DeleteObject(const FString& URL) { return false; };
	virtual bool PostObject(const FString& URL) { return false; };

	virtual void HttpRequestComplete(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bConnecteDS_ccess);
	virtual void HttpRequestProgress(FHttpRequestPtr InRequest, int32 BytesSent, int32 BytesReceived);
	virtual void HttpRequestHeaderReceived(FHttpRequestPtr InRequest, const FString& HeaderName, const FString& NewHeaderValue);

protected:
	virtual void ExecutionCompleteDelegate(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bConnecteDS_ccess);

	FString TmpSavePaths;
	bool bRequestComplete;
	bool bSaveDisk;

public:
	FORCEINLINE FString GetPaths() { return TmpSavePaths; }
	FORCEINLINE void SetPaths(const FString& NewPaths) { TmpSavePaths = NewPaths; }
	FORCEINLINE bool IsRequestComplete() { return bRequestComplete; }
};