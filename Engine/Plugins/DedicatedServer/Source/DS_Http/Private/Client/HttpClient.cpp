#include "Client/HttpClient.h"
#include "Definition/HttpMacro.h"
using namespace DS_Http;


DS_Http::FHttpPutObjectRequest::FHttpPutObjectRequest(const FString& URL, const FString& ContentString)
{
	DEFINITION_HTTP_TYPE(PUT, "multipart/form-data;charset=utf-8");
	HttpRequest->SetContentAsString(ContentString);
}

DS_Http::FHttpPutObjectRequest::FHttpPutObjectRequest(const FString& URL, const TArray<uint8>& ContentPayload)
{
	DEFINITION_HTTP_TYPE(PUT, "multipart/form-data;charset=utf-8");
	HttpRequest->SetContent(ContentPayload);
}

DS_Http::FHttpPutObjectRequest::FHttpPutObjectRequest(const FString& URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream)
{
	DEFINITION_HTTP_TYPE(PUT, "multipart/form-data;charset=utf-8");
	HttpRequest->SetContentFromStream(Stream);
}

DS_Http::FHttpGetObjectRequest::FHttpGetObjectRequest(const FString& URL)
{
	DEFINITION_HTTP_TYPE(GET, "application/x-www-form-urlencoded;charset=utf-8");
}

DS_Http::FHttpDeleteObjectsRequest::FHttpDeleteObjectsRequest(const FString& URL)
{
	DEFINITION_HTTP_TYPE(DELETE, "application/x-www-form-urlencoded;charset=utf-8");
}

DS_Http::FHttpPostObjectsRequest::FHttpPostObjectsRequest(const FString& URL)
{
	DEFINITION_HTTP_TYPE(POST, "application/x-www-form-urlencoded;charset=utf-8");
}
