#pragma once

#include "HttpType.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

namespace DSUHttp
{
	void RequestPtrToRequest(FHttpRequestPtr Request, FHttpRequest& HttpRequest)
	{
		if (Request.IsValid())
		{
			HttpRequest.Verb = Request->GetVerb();
			HttpRequest.URL = Request->GetURL();
			HttpRequest.Status = (EHttpState)Request->GetStatus();
			HttpRequest.ElapsedTime = Request->GetElapsedTime();
			HttpRequest.ContentType = Request->GetContentType();
			HttpRequest.ContentLength = Request->GetContentLength();
			HttpRequest.AllHeaders = Request->GetAllHeaders();
		}
	}

	void ResponsePtrToResponse(FHttpResponsePtr Response, FHttpResponse& HttpResponse)
	{
		if (Response.IsValid())
		{
			HttpResponse.ResponseCode = Response->GetResponseCode();
			HttpResponse.URL = Response->GetURL();
			HttpResponse.ResponseMessage = Response->GetContentAsString();
			HttpResponse.ContentType = Response->GetContentType();
			HttpResponse.ContentLength = Response->GetContentLength();
			HttpResponse.AllHeaders = Response->GetAllHeaders();
			HttpResponse.Content->Content = const_cast<TArray<uint8>*>(&Response->GetContent());
		}
	}

	// 判断一个UTF8字符是否属于合法字符集（A-Z, a-z, 0-9, -+=_.~:/#@?&）
	// 合法字符可直接放入URL，非法字符需要进行URL编码(对‘%’进行转义)
	bool IsLegitimateChar(UTF8CHAR LookupChar)
	{
		//Infrastructure construction
		static int8 TemplateChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-+=_.~:/#@?&";
		static bool LegitimateTable[256] = { 0 };
		static bool bTableAlreadyUsed = false;
		if (!bTableAlreadyUsed)
		{
			//INDEX_NONE to avoid trailing 0
			for (int32 i = 0; i < UE_ARRAY_COUNT(TemplateChars) - INDEX_NONE; ++i)
			{
				uint8 LegitimateTableIndex = static_cast<uint8>(TemplateChars[i]);
				check(LegitimateTableIndex < UE_ARRAY_COUNT(LegitimateTable));
				LegitimateTable[LegitimateTableIndex] = true;
			}
			bTableAlreadyUsed = true;
		}
		return LegitimateTable[LookupChar];
	}

	// 若数据包里有中文字符，进行特殊处理
	FString EncodeURL(const TCHAR* InUnencodedString)
	{
		FString OutEncodedString;
		TCHAR PreallocationBuffer[2] = { 0 };

		FTCHARToUTF8 UTF8Converter(InUnencodedString);
		const UTF8CHAR* UTF8ByteData = (UTF8CHAR*)UTF8Converter.Get();

		for (int32 i = 0; i < UTF8Converter.Length(); ++i)
		{
			UTF8CHAR ByteToEncode = UTF8ByteData[i];
			if (IsLegitimateChar(ByteToEncode))
			{
				PreallocationBuffer[0] = ByteToEncode;
				OutEncodedString += FString(PreallocationBuffer);
			}
			else if (ByteToEncode != 0)
			{
				OutEncodedString += (TEXT("%") + FString::Printf(TEXT("%.2X"), ByteToEncode));
			}
		}
		return OutEncodedString;
	}
}


#define DEFINITION_HTTP_TYPE(VerbString,Content) \
FString InNewURLEncoded = DSUHttp::EncodeURL(*URL); \
HttpRequest->SetURL(InNewURLEncoded); \
HttpRequest->SetVerb(TEXT(#VerbString)); \
HttpRequest->SetHeader(TEXT("Content-Type"), TEXT(#Content));

#define REQUEST_BIND_FUNCTION(RequestClass) \
(*Request) \
<< FHttpRequestHeaderReceivedDelegate::CreateRaw(this, &RequestClass::HttpRequestHeaderReceived) \
<< FHttpRequestProgressDelegate::CreateRaw(this, &RequestClass::HttpRequestProgress) \
<< FHttpRequestCompleteDelegate::CreateRaw(this, &RequestClass::HttpRequestComplete);


#define HTTP_REGISTERED_REQUEST_BP(TYPE) \
auto Handle = RegisteredHttpRequest(TYPE, \
	ResponseDelegateBP.OnRequestCompleteDelegate, \
	ResponseDelegateBP.OnRequestProgressDelegate, \
	ResponseDelegateBP.OnRequestHeaderReceivedDelegate, \
	ResponseDelegateBP.OnAllRequestCompleteDelegate); \
TmpStorageHandle = Handle;

#define HTTP_REGISTERED_REQUEST(TYPE) \
auto Handle = RegisteredHttpRequest(TYPE, \
	ResponseDelegate.OnCompleteDelegate, \
	ResponseDelegate.OnRequestProgressDelegate, \
	ResponseDelegate.OnRequestHeaderReceivedDelegate, \
	ResponseDelegate.OnAllTasksCompletedDelegate); \
TmpStorageHandle = Handle;

