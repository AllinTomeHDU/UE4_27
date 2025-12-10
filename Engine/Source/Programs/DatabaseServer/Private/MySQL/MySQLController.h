#pragma once

#include "CoreMinimal.h"
#include "DedicatedServerUtils/NetChannel/UObject/NetChannelController.h"
#include "DedicatedServerUtils/Database/MySQL/Link/MySQL_LinkType.h"
#include "MySQLController.generated.h"

class UMySQL_Object;


UCLASS()
class DEDICATEDSERVERUTILS_API UMySQLController : public UNetChannelController
{
	GENERATED_BODY()

public:
	bool Post(const FString& InSQL);
	bool Get(const FString& InSQL, TArray<FMySQL_FieldsData>& Results);

protected:
	virtual void Init() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Close() override;
	virtual void RecvProtocol(uint32 InProtocol) override;

private:
	UMySQL_Object* ObjectRead;
	UMySQL_Object* ObjectWrite;
};