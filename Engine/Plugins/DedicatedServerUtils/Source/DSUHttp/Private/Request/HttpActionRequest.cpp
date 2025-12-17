#include "Request/HttpActionRequest.h"
#include "Core/HttpMacro.h"
using namespace DSUHttp;


void FHttpActionRequest::HttpRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccess)
{
	FString DebugPram;
	Request->GetURLParameter(DebugPram);
	UE_LOG(LogTemp, Display, TEXT("HttpRequestComplete URL=%s,Param=%s"), *Request->GetURL(), *DebugPram);

	if (!Request.IsValid())
	{
		ExecutionCompleteDelegate(Request, Response, bConnectedSuccess);
		UE_LOG(LogTemp, Display, TEXT("Server request failed."));
	}
	else if (!Response.IsValid())
	{
		ExecutionCompleteDelegate(Request, Response, bConnectedSuccess);
		UE_LOG(LogTemp, Display, TEXT("Response failed."));
	}
	else if (!bConnectedSuccess)
	{
		ExecutionCompleteDelegate(Request, Response, bConnectedSuccess);
		UE_LOG(LogTemp, Display, TEXT("HTTP connected failed. msg[%s]"), *Response->GetContentAsString());
	}
	else if (!EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		ExecutionCompleteDelegate(Request, Response, bConnectedSuccess);
		UE_LOG(LogTemp, Warning, TEXT("HTTP error code [%d] Msg[%s]."),
			Response->GetResponseCode(), *Response->GetContentAsString());
	}
	else
	{
		if (Request->GetVerb() == "GET")
		{
			if (bSaveDisk)
			{
				FString Filename = FPaths::GetCleanFilename(Request->GetURL());
				FFileHelper::SaveArrayToFile(Response->GetContent(), *(GetPaths() / Filename));
				UE_LOG(LogTemp, Log, TEXT("Store the obtained http file locally: [%s]"), *Filename);
			}
		}
		ExecutionCompleteDelegate(Request, Response, bConnectedSuccess);
		UE_LOG(LogTemp, Log, TEXT("Request to complete execution of binding agent."));
	}
}

void FHttpActionRequest::HttpRequestProgress(FHttpRequestPtr InRequest, int32 BytesSent, int32 BytesReceived)
{
	FHttpRequest HttpRequest;
	RequestPtrToRequest(InRequest, HttpRequest);

	OnRequestProgressDelegate.ExecuteIfBound(HttpRequest, BytesSent, BytesReceived);
	OnHttpRequestProgressDelegateBP.ExecuteIfBound(HttpRequest, BytesSent, BytesReceived);
}

void FHttpActionRequest::HttpRequestHeaderReceived(FHttpRequestPtr InRequest, const FString& HeaderName, const FString& NewHeaderValue)
{
	FHttpRequest HttpRequest;
	RequestPtrToRequest(InRequest, HttpRequest);

	OnRequestHeaderReceivedDelegate.ExecuteIfBound(HttpRequest, HeaderName, NewHeaderValue);
	OnHttpRequestHeaderReceivedDelegateBP.ExecuteIfBound(HttpRequest, HeaderName, NewHeaderValue);
}

void FHttpActionRequest::ExecutionCompleteDelegate(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bConnectedSuccess)
{
	FHttpRequest HttpRequest;
	FHttpResponse HttpResponse;
	ResponsePtrToResponse(InResponse, HttpResponse);
	RequestPtrToRequest(InRequest, HttpRequest);

	OnCompleteDelegate.ExecuteIfBound(HttpRequest, HttpResponse, bConnectedSuccess);
	OnHttpRequestCompleteDelegateBP.ExecuteIfBound(HttpRequest, HttpResponse, bConnectedSuccess);
}
