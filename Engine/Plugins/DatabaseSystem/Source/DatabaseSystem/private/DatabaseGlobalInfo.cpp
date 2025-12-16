#include "DatabaseGlobalInfo.h"
#include "Misc/FileHelper.h"


FMySQLGlobalInfo* FMySQLGlobalInfo::Instance = nullptr;


FMySQLGlobalInfo* FMySQLGlobalInfo::Get()
{
	if (!Instance)
	{
		Instance = new FMySQLGlobalInfo();
	}
	return Instance;
}

void FMySQLGlobalInfo::Destroy()
{
	delete Instance;
	Instance = nullptr;
}

void FMySQLGlobalInfo::Init(const FString& InPath)
{
	TArray<FString> Content;
	FFileHelper::LoadFileToStringArray(Content, *InPath);
	if (Content.Num())
	{
		auto AnalysisConfig = [&](TMap<FString, FString>& OutContent)
		{
			for (auto& Tmp : Content)
			{
				if (Tmp.Contains(TEXT("[")) && Tmp.Contains(TEXT("]")))
				{
					Tmp.RemoveFromEnd("]");
					Tmp.RemoveFromStart("[");
					OutContent.Add("ConfigHead", Tmp);
				}
				else if (Tmp.Contains("="))
				{
					FString L, R;
					Tmp.Split(TEXT("="), &L, &R);
					OutContent.Add(L, R);
				}
			}
		};
		TMap<FString, FString> InConfigContent;
		AnalysisConfig(InConfigContent);

		/*
		* 如果有多个数据库信息，需要修改 Lambda 函数，根据标题检索配置，
		* 跳过非标题下的配置信息，只传出指定标题参数的配置信息
		* 示例：AnalysisConfig(MySQLConfigContent, TEXT("MySQL"));
		*/

		MySQLConfig.Host = InConfigContent["Host"];
		MySQLConfig.User = InConfigContent["User"];
		MySQLConfig.Password = InConfigContent["Password"];
		MySQLConfig.Port = FCString::Atoi(*InConfigContent["Port"]);
		MySQLConfig.Database = InConfigContent["Database"];
	}
	else
	{
		Content.Add(TEXT("MySQL"));
		Content.Add(FString::Printf(TEXT("Host=%s"), *MySQLConfig.Host));
		Content.Add(FString::Printf(TEXT("User=%s"), *MySQLConfig.User));
		Content.Add(FString::Printf(TEXT("Password=%s"), *MySQLConfig.Password));
		Content.Add(FString::Printf(TEXT("Port=%i"), MySQLConfig.Port));
		Content.Add(FString::Printf(TEXT("Database=%s"), *MySQLConfig.Database));
		Content.Add(TEXT(""));
		FFileHelper::SaveStringArrayToFile(Content, *InPath);
	}
}
