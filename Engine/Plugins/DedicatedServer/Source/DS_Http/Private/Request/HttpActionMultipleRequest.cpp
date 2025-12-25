#include "Request/HttpActionMultipleRequest.h"
#include "Definition/HttpMacro.h"
#include "Client/HttpClient.h"


bool FHttpActionMultipleRequest::Suspend()
{
	return false;
}

bool FHttpActionMultipleRequest::Cancel()
{
	for (auto& Tmp : Requests)
	{
		if (Tmp.IsValid())
		{
			FHttpClient().Cancel(Tmp.ToSharedRef());
		}
	}
	return true;
}

void FHttpActionMultipleRequest::GetObjects(const TArray<FString>& URL, const FString& SavePaths)
{
	bSaveDisk = !SavePaths.IsEmpty();
	TmpSavePaths = SavePaths;
	for (const auto& Tmp : URL)
	{
		Requests.Add(MakeShareable(new FHttpGetObjectRequest(Tmp)));
		TSharedPtr<IHttpClientRequest> Request = Requests.Last();
		REQUEST_BIND_FUNCTION(FHttpActionMultipleRequest);
		if (FHttpClient().Execute(Request.ToSharedRef()))
		{
			RequestNumber++;
		}
	}
}

void FHttpActionMultipleRequest::DeleteObjects(const TArray<FString>& URL)
{
	for (const auto& Tmp : URL)
	{
		Requests.Add(MakeShareable(new FHttpDeleteObjectsRequest(Tmp)));
		TSharedPtr<IHttpClientRequest> Request = Requests.Last();
		REQUEST_BIND_FUNCTION(FHttpActionMultipleRequest);
		if (FHttpClient().Execute(Request.ToSharedRef()))
		{
			RequestNumber++;
		}
	}
}

bool FHttpActionMultipleRequest::PutObject(const FString& URL, const FString& LocalPaths)
{
	TmpSavePaths = LocalPaths;

	TArray<FString> AllPaths;
	IFileManager::Get().FindFilesRecursive(AllPaths, *LocalPaths, TEXT("*"), true, false);
	if (!AllPaths.Num())
	{
		UE_LOG(LogTemp, Display, TEXT("Error: The obtained content is empty.[Path:%s]"), *LocalPaths);
		return false;
	}

	for (const auto& Tmp : AllPaths)
	{
		TArray<uint8> ByteData;
		FFileHelper::LoadFileToArray(ByteData, *Tmp);

		FString ObjectName = FPaths::GetCleanFilename(Tmp);
		Requests.Add(MakeShareable(new FHttpPutObjectRequest(URL / ObjectName, ByteData)));
		TSharedPtr<IHttpClientRequest> Request = Requests.Last();
		REQUEST_BIND_FUNCTION(FHttpActionMultipleRequest);
		if (FHttpClient().Execute(Request.ToSharedRef()))
		{
			RequestNumber++;
		}
	}
	return RequestNumber > 0;
}

void FHttpActionMultipleRequest::ExecutionCompleteDelegate(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bConnectedSuccess)
{
	FHttpActionRequest::ExecutionCompleteDelegate(InRequest, InResponse, bConnectedSuccess);

	if (RequestNumber > 0)
	{
		RequestNumber--;
		if (RequestNumber <= 0)
		{
			OnAllTasksCompletedDelegate.ExecuteIfBound();
			OnAllRequestCompleteDelegateBP.ExecuteIfBound();
			bRequestComplete = true;
		}
	}
}

