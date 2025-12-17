#include "Request/HttpRequestInterface.h"
#include "HttpModule.h"
using namespace DSUHttp;


IHttpClientRequest::IHttpClientRequest()
	: HttpRequest(FHttpModule::Get().CreateRequest())
{
}

IHttpClientRequest& DSUHttp::IHttpClientRequest::operator<<(const FHttpRequestCompleteDelegate& InDelegate)
{
	HttpRequest->OnProcessRequestComplete() = InDelegate;
	return *this;
}

IHttpClientRequest& DSUHttp::IHttpClientRequest::operator<<(const FHttpRequestProgressDelegate& InDelegate)
{
	HttpRequest->OnRequestProgress() = InDelegate;
	return *this;
}

IHttpClientRequest& DSUHttp::IHttpClientRequest::operator<<(const FHttpRequestHeaderReceivedDelegate& InDelegate)
{
	HttpRequest->OnHeaderReceived() = InDelegate;
	return *this;
}
