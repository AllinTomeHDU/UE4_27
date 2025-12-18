#pragma once

#include "HttpActionRequest.h"
#include "HttpRequestInterface.h"


struct DS_HTTP_API FHttpActionSingleRequest : public FHttpActionRequest
{
public:
	FHttpActionSingleRequest() : FHttpActionRequest(), Request(nullptr) {}

	virtual bool Suspend() override;
	virtual bool Cancel() override;

	virtual bool GetObject(const FString& URL, const FString& SavePaths) override;
	virtual bool PutObject(const FString& URL, const TArray<uint8>& Data) override;
	virtual bool PutObject(const FString& URL, const FString& LocalPaths) override;
	virtual bool PutObject(const FString& URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream) override;
	virtual bool PutObjectByString(const FString& URL, const FString& InBuff) override;
	virtual bool DeleteObject(const FString& URL) override;
	virtual bool PostObject(const FString& URL) override;

protected:
	virtual void ExecutionCompleteDelegate(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bConnecteDS_ccess) override;

private:
	TSharedPtr<DS_Http::IHttpClientRequest> Request;
};