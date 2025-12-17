#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"


namespace DSUHttp
{
	class IHttpClientRequest : public TSharedFromThis<IHttpClientRequest>
	{
		friend struct FHttpClient;
	public:
		IHttpClientRequest();

		IHttpClientRequest& operator<<(const FHttpRequestCompleteDelegate& InDelegate);
		IHttpClientRequest& operator<<(const FHttpRequestProgressDelegate& InDelegate);
		IHttpClientRequest& operator<<(const FHttpRequestHeaderReceivedDelegate& InDelegate);

	protected:
		FORCEINLINE bool ProcessRequest() { return HttpRequest->ProcessRequest(); }
		FORCEINLINE void CancelRequest() { HttpRequest->CancelRequest(); }

	protected:
		TSharedPtr<class IHttpRequest, ESPMode::ThreadSafe> HttpRequest;  // 4.26以上的版本
	};
}