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
					if (Tmp.Contains("="))
					{
						FString L, R;
						Tmp.Split(TEXT("="), &L, &R);
						OutContent.Add(L, R);
					}
				}
			};
		TMap<FString, FString> InConfigContent;
		AnalysisConfig(InConfigContent);
		ConfigInfo.Host = InConfigContent["Host"];
		ConfigInfo.User = InConfigContent["User"];
		ConfigInfo.Password = InConfigContent["Password"];
		ConfigInfo.Port = FCString::Atoi(*InConfigContent["Port"]);
		ConfigInfo.Database = InConfigContent["Database"];
	}
	else
	{
		Content.Add(FString::Printf(TEXT("Host=%s"), *ConfigInfo.Host));
		Content.Add(FString::Printf(TEXT("User=%s"), *ConfigInfo.User));
		Content.Add(FString::Printf(TEXT("Password=%s"), *ConfigInfo.Password));
		Content.Add(FString::Printf(TEXT("Port=%i"), ConfigInfo.Port));
		Content.Add(FString::Printf(TEXT("Database=%s"), *ConfigInfo.Database));
		FFileHelper::SaveStringArrayToFile(Content, *InPath);
	}
}
