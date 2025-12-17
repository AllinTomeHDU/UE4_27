#include "Client/HttpClient.h"
#include "Core/HttpMacro.h"
using namespace DSUHttp;


DSUHttp::FHttpPutObjectRequest::FHttpPutObjectRequest(const FString& URL, const FString& ContentString)
{
	DEFINITION_HTTP_TYPE(PUT, "multipart/form-data;charset=utf-8");
	HttpRequest->SetContentAsString(ContentString);
}

DSUHttp::FHttpPutObjectRequest::FHttpPutObjectRequest(const FString& URL, const TArray<uint8>& ContentPayload)
{
	DEFINITION_HTTP_TYPE(PUT, "multipart/form-data;charset=utf-8");
	HttpRequest->SetContent(ContentPayload);
}

DSUHttp::FHttpPutObjectRequest::FHttpPutObjectRequest(const FString& URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream)
{
	DEFINITION_HTTP_TYPE(PUT, "multipart/form-data;charset=utf-8");
	HttpRequest->SetContentFromStream(Stream);
}

DSUHttp::FHttpGetObjectRequest::FHttpGetObjectRequest(const FString& URL)
{
	DEFINITION_HTTP_TYPE(GET, "application/x-www-form-urlencoded;charset=utf-8");
}

DSUHttp::FHttpDeleteObjectsRequest::FHttpDeleteObjectsRequest(const FString& URL)
{
	DEFINITION_HTTP_TYPE(DELETE, "application/x-www-form-urlencoded;charset=utf-8");
}

DSUHttp::FHttpPostObjectsRequest::FHttpPostObjectsRequest(const FString& URL)
{
	DEFINITION_HTTP_TYPE(POST, "application/x-www-form-urlencoded;charset=utf-8");
}
