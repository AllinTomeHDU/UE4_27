#pragma once

#include "HttpActionRequest.h"
#include "HttpRequestInterface.h"


struct DS_HTTP_API FHttpActionMultipleRequest : public FHttpActionRequest
{
public:
	FHttpActionMultipleRequest() : FHttpActionRequest(), RequestNumber(0) {}

	virtual bool Suspend() override;
	virtual bool Cancel() override;

	virtual void GetObjects(const TArray<FString>& URL, const FString& SavePaths) override;
	virtual void DeleteObjects(const TArray<FString>& URL) override;
	virtual bool PutObject(const FString& URL, const FString& LocalPaths) override;

protected:
	virtual void ExecutionCompleteDelegate(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bConnecteDS_ccess) override;

private:
	uint32 RequestNumber;
	TArray<TSharedPtr<DS_Http::IHttpClientRequest>> Requests;
};