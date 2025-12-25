#include "Request/HttpActionSingleRequest.h"
#include "Client/HttpClient.h"
#include "Definition/HttpMacro.h"


bool FHttpActionSingleRequest::Suspend()
{
	return false;
}

bool FHttpActionSingleRequest::Cancel()
{
	if (Request.IsValid())
	{
		FHttpClient().Cancel(Request.ToSharedRef());
		return true;
	}
	return false;
}

bool FHttpActionSingleRequest::GetObject(const FString& URL, const FString& SavePaths)
{
	bSaveDisk = !SavePaths.IsEmpty();
	TmpSavePaths = SavePaths;
	Request = MakeShareable(new FHttpGetObjectRequest(URL));
	REQUEST_BIND_FUNCTION(FHttpActionSingleRequest);
	return FHttpClient().Execute(Request.ToSharedRef());
}

bool FHttpActionSingleRequest::PutObject(const FString& URL, const TArray<uint8>& Data)
{
	Request = MakeShareable(new FHttpPutObjectRequest(URL, Data));
	REQUEST_BIND_FUNCTION(FHttpActionSingleRequest);
	return FHttpClient().Execute(Request.ToSharedRef());
}

bool FHttpActionSingleRequest::PutObject(const FString& URL, const FString& LocalPaths)
{
	TArray<uint8> Data;
	if (FFileHelper::LoadFileToArray(Data, *LocalPaths))
	{
		Request = MakeShareable(new FHttpPutObjectRequest(URL, Data));
		REQUEST_BIND_FUNCTION(FHttpActionSingleRequest);
		return FHttpClient().Execute(Request.ToSharedRef());
	}
	return false;
}

bool FHttpActionSingleRequest::PutObject(const FString& URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream)
{
	Request = MakeShareable(new FHttpPutObjectRequest(URL, Stream));
	REQUEST_BIND_FUNCTION(FHttpActionSingleRequest);
	return FHttpClient().Execute(Request.ToSharedRef());
}

bool FHttpActionSingleRequest::PutObjectByString(const FString& URL, const FString& InBuff)
{
	Request = MakeShareable(new FHttpPutObjectRequest(URL, InBuff));
	REQUEST_BIND_FUNCTION(FHttpActionSingleRequest);
	return FHttpClient().Execute(Request.ToSharedRef());
}

bool FHttpActionSingleRequest::DeleteObject(const FString& URL)
{
	Request = MakeShareable(new FHttpDeleteObjectsRequest(URL));
	return FHttpClient().Execute(Request.ToSharedRef());
}

bool FHttpActionSingleRequest::PostObject(const FString& URL)
{
	bSaveDisk = false;
	Request = MakeShareable(new FHttpPostObjectsRequest(URL));
	REQUEST_BIND_FUNCTION(FHttpActionSingleRequest);
	return FHttpClient().Execute(Request.ToSharedRef());
}

void FHttpActionSingleRequest::ExecutionCompleteDelegate(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bConnectedSuccess)
{
	FHttpActionRequest::ExecutionCompleteDelegate(InRequest, InResponse, bConnectedSuccess);

	OnAllTasksCompletedDelegate.ExecuteIfBound();
	OnAllRequestCompleteDelegateBP.ExecuteIfBound();
	bRequestComplete = true;
}

