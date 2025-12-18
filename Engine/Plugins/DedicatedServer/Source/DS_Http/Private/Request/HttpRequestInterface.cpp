#include "Request/HttpRequestInterface.h"
#include "HttpModule.h"
using namespace DS_Http;


IHttpClientRequest::IHttpClientRequest()
	: HttpRequest(FHttpModule::Get().CreateRequest())
{
}

IHttpClientRequest& DS_Http::IHttpClientRequest::operator<<(const FHttpRequestCompleteDelegate& InDelegate)
{
	HttpRequest->OnProcessRequestComplete() = InDelegate;
	return *this;
}

IHttpClientRequest& DS_Http::IHttpClientRequest::operator<<(const FHttpRequestProgressDelegate& InDelegate)
{
	HttpRequest->OnRequestProgress() = InDelegate;
	return *this;
}

IHttpClientRequest& DS_Http::IHttpClientRequest::operator<<(const FHttpRequestHeaderReceivedDelegate& InDelegate)
{
	HttpRequest->OnHeaderReceived() = InDelegate;
	return *this;
}
