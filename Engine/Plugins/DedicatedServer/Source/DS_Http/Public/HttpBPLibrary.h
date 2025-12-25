#pragma once

#include "CoreMinimal.h"
#include "Definition/HttpType.h"
#include "HttpBPLibrary.generated.h"


UCLASS(meta = (BlueprintThreadSafe, ScriptName = "HttpLibrary"))
class DS_HTTP_API UHttpBPLibrary : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "HTTP")
	static FName GetHandleByLastExecutionRequest();

	UFUNCTION(BlueprintCallable, Category = "HTTP")
	static void Pause();

	UFUNCTION(BlueprintCallable, Category = "HTTP")
	static void Awaken();

	UFUNCTION(BlueprintCallable, Category = "HTTP")
	static bool Cancel();

	UFUNCTION(BlueprintCallable, Category = "HTTP")
	static bool CancelByHandle(const FName& Handle);

	UFUNCTION(BlueprintCallable, Category = "HTTP|SingleAction")
	static bool PostRequest(const FString& InURL, const FString& InParam, const FHttpResponseDelegateBP& ResponseDelegateBP);

	UFUNCTION(BlueprintCallable, Category = "HTTP|SingleAction")
	static bool GetObjectToMemory(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL);

	UFUNCTION(BlueprintCallable, Category = "HTTP|MultpleAction")
	static void GetObjectsToMemory(const FHttpResponseDelegateBP& ResponseDelegateBP, const TArray<FString>& URL);

	UFUNCTION(BlueprintCallable, Category = "HTTP|SingleAction")
	static bool GetObjectToLocal(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const FString& SavePaths);

	UFUNCTION(BlueprintCallable, Category = "HTTP|MultpleAction")
	static void GetObjectsToLocal(const FHttpResponseDelegateBP& ResponseDelegateBP, const TArray<FString>& URL, const FString& SavePaths);

	UFUNCTION(BlueprintCallable, Category = "HTTP|SingleAction")
	static bool PutObjectFromLocal(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const FString& LocalPaths);

	UFUNCTION(BlueprintCallable, Category = "HTTP|MultpleAction")
	static bool PutObjectsFromLocal(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const FString& LocalPaths);

	UFUNCTION(BlueprintCallable, Category = "HTTP|SingleAction")
	static bool PutObjectFromBuffer(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const TArray<uint8>& Buffer);

	UFUNCTION(BlueprintCallable, Category = "HTTP|SingleAction")
	static bool PutObjectFromString(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL, const FString& InString);

	UFUNCTION(BlueprintCallable, Category = "HTTP|SingleAction")
	static bool DeleteObject(const FHttpResponseDelegateBP& ResponseDelegateBP, const FString& URL);

	UFUNCTION(BlueprintCallable, Category = "HTTP|MultpleAction")
	static void DeleteObjects(const FHttpResponseDelegateBP& ResponseDelegateBP, const TArray<FString>& URL);

	UFUNCTION(BlueprintCallable, Category = "HTTP|SingleAction")
	static void Tick(float DeltaTime);

};