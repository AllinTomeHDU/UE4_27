#pragma once

#include "CoreMinimal.h"
#include "../Request/HttpRequestInterface.h"

using namespace DS_Http;
namespace DS_Http
{
	// Get:		从服务器获取资源
	// Post:	在服务器创建资源
	// Put:		在服务器上更新资源，并由客户端提供完整的已修改资源
	// Patch:	在服务器上更新资源，但客户端只需提供修改过的部分属性
	// Delete:	从服务器上删除资源
	// Head:	获取资源的元数据（不返回资源本身内容）
	// Options: 获取服务器支持的操作信息，例如客户端可以修改哪些属性

	struct FHttpPutObjectRequest : IHttpClientRequest
	{
		FHttpPutObjectRequest(const FString& URL, const FString& ContentString);
		FHttpPutObjectRequest(const FString& URL, const TArray<uint8>& ContentPayload);
		FHttpPutObjectRequest(const FString& URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream);
	};

	struct FHttpGetObjectRequest : IHttpClientRequest
	{
		FHttpGetObjectRequest(const FString& URL);
	};

	struct FHttpDeleteObjectsRequest :IHttpClientRequest
	{
		FHttpDeleteObjectsRequest(const FString& URL);
	};

	struct FHttpPostObjectsRequest :IHttpClientRequest
	{
		FHttpPostObjectsRequest(const FString& URL);
	};

	struct FHttpClient
	{
		FHttpClient() {}
		FHttpClient(TSharedPtr<IHttpClientRequest> InHttpRequest)
		{
			Execute(InHttpRequest.ToSharedRef());
		}

		bool Execute(TSharedRef<IHttpClientRequest> InHttpRequest) const
		{
			return InHttpRequest->ProcessRequest();
		}
		void Cancel(TSharedRef<IHttpClientRequest> InHttpRequest)
		{
			InHttpRequest->CancelRequest();
		}
	};
}