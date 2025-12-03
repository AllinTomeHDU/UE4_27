#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MySQL_Example.generated.h"

#define TEST_PASSWORD TEXT("20212526Hdu.")


UCLASS()
class DEDICATEDSERVERUTILS_API  UMySQL_Example : public UObject
{
	GENERATED_BODY()

public:
	static void TestMySQL_1();	// 简单测试连接、查询
	static void TestMySQL_2();	// 预编译查询
	static void TestMySQL_3();	// SQL基本知识
	static void TestMySQL_4();	// 测试封装的MySQL_Link
};
