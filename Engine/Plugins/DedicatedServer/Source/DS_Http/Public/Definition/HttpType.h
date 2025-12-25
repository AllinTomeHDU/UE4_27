#pragma once

#include "CoreMinimal.h"
#include "Misc/FileHelper.h"
#include "HttpType.generated.h"


UCLASS(BlueprintType)
class DS_HTTP_API UHttpContentObject :public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "HTTP|HttpContent")
	FORCEINLINE TArray<uint8>& GetContent() { return *Content; }

	UFUNCTION(BlueprintPure, Category = "HTTP|HttpContent")
	FORCEINLINE bool Save(const FString& LocalPath) { return FFileHelper::SaveArrayToFile(*Content, *LocalPath); }

	TArray<uint8>* Content;
};

enum class EHttpRequestType
{
	SINGLE,
	MULTPLE,
};

UENUM(BlueprintType)
enum class EHttpState : uint8
{
	NotStarted,
	Processing,
	Failed,
	Failed_ConnectionError,
	Succeeded
};

USTRUCT(BlueprintType)
struct DS_HTTP_API FHttpContentBase
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HTTP|HttpBase")
	FString URL;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HTTP|HttpBase")
	TArray<FString> AllHeaders;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HTTP|HttpBase")
	FString ContentType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HTTP|HttpBase")
	int32 ContentLength;
};

USTRUCT(BlueprintType)
struct DS_HTTP_API FHttpRequest : public FHttpContentBase
{
	GENERATED_BODY()

	FHttpRequest() : ElapsedTime(INDEX_NONE), Status(EHttpState::NotStarted) {}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HTTP|HttpRequest")
	FString Verb;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HTTP|HttpRequest")
	float ElapsedTime = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HTTP|HttpRequest")
	EHttpState Status;
};

USTRUCT(BlueprintType)
struct DS_HTTP_API FHttpResponse : public FHttpContentBase
{
	GENERATED_BODY()

	FHttpResponse() : ResponseCode(INDEX_NONE), Content(NewObject<UHttpContentObject>()){}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HTTP|HttpResponse")
	int32 ResponseCode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HTTP|HttpResponse")
	FString ResponseMessage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HTTP|HttpResponse")
	UHttpContentObject* Content;
};

//C++
DECLARE_DELEGATE_ThreeParams(FOnCompleteDelegate, const FHttpRequest&, const FHttpResponse&, bool);
DECLARE_DELEGATE_ThreeParams(FOnRequestProgressDelegate, const FHttpRequest&, int64, int64);
DECLARE_DELEGATE_ThreeParams(FOnRequestHeaderReceivedDelegate, const FHttpRequest&, const FString&, const FString&);

//BP
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnRequestCompleteDelegateBP, const FHttpRequest, Request, const FHttpResponse, Response, bool, bConnecteDS_ccessfully);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnRequestProgressDelegateBP, const FHttpRequest, Request, int64, BytesSent, int64, BytesReceived);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnRequestHeaderReceivedDelegateBP, const FHttpRequest, Request, const FString, HeaderName, const FString, NewHeaderValue);
DECLARE_DYNAMIC_DELEGATE(FOnAllRequestsCompleteDelegateBP);

USTRUCT(BlueprintType)
struct DS_HTTP_API FHttpResponseDelegateBP
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HTTP|HttpResponseBPDelegate")
	FOnRequestCompleteDelegateBP OnRequestCompleteDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HTTP|HttpResponseBPDelegate")
	FOnRequestProgressDelegateBP OnRequestProgressDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HTTP|HttpResponseBPDelegate")
	FOnRequestHeaderReceivedDelegateBP OnRequestHeaderReceivedDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HTTP|HttpResponseBPDelegate")
	FOnAllRequestsCompleteDelegateBP OnAllRequestCompleteDelegate;
};

struct DS_HTTP_API FHttpResponseDelegate
{
	FOnCompleteDelegate OnCompleteDelegate;
	FOnRequestProgressDelegate OnRequestProgressDelegate;
	FOnRequestHeaderReceivedDelegate OnRequestHeaderReceivedDelegate;
	FSimpleDelegate OnAllTasksCompletedDelegate;
};