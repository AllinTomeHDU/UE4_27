#include "NetChannelGlobalInfo.h"
#include "Misc/FileHelper.h"


FNetChannelGlobalInfo* FNetChannelGlobalInfo::Instance = nullptr;

FNetChannelGlobalInfo* FNetChannelGlobalInfo::Get()
{
	if (!Instance)
	{
		Instance = new FNetChannelGlobalInfo();
	}
	return Instance;
}

void FNetChannelGlobalInfo::Destroy()
{
	delete Instance;
	Instance = nullptr;
}

void FNetChannelGlobalInfo::Init(const FString& InPath)
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
		ConfigInfo.Version = InConfigContent["Version"];
		ConfigInfo.URL = InConfigContent["URL"];
		ConfigInfo.Port = FCString::Atoi(*InConfigContent["Port"]);
		ConfigInfo.RecvDataSize = FCString::Atoi(*InConfigContent["RecvDataSize"]);
		ConfigInfo.SendDataSize = FCString::Atoi(*InConfigContent["SendDataSize"]);
		ConfigInfo.MaxConnections = FCString::Atoi(*InConfigContent["MaxConnections"]);
		ConfigInfo.MaxChannels = FCString::Atoi(*InConfigContent["MaxChannels"]);
		ConfigInfo.HeartBeatTimeInterval = FCString::Atof(*InConfigContent["HeartBeatTimeInterval"]);
		ConfigInfo.HeatBeatTimeOutTime = FCString::Atof(*InConfigContent["HeatBeatTimeOutTime"]);
	}
	else
	{
		Content.Add(FString::Printf(TEXT("Version=%s"), *ConfigInfo.Version));
		Content.Add(FString::Printf(TEXT("URL=%s"), *ConfigInfo.URL));
		Content.Add(FString::Printf(TEXT("Port=%i"), ConfigInfo.Port));
		Content.Add(FString::Printf(TEXT("RecvDataSize=%i"), ConfigInfo.RecvDataSize));
		Content.Add(FString::Printf(TEXT("SendDataSize=%i"), ConfigInfo.SendDataSize));
		Content.Add(FString::Printf(TEXT("MaxConnections=%i"), ConfigInfo.MaxConnections));
		Content.Add(FString::Printf(TEXT("MaxChannels=%i"), ConfigInfo.MaxChannels));
		Content.Add(FString::Printf(TEXT("HeartBeatTimeInterval=%f"), ConfigInfo.HeartBeatTimeInterval));
		Content.Add(FString::Printf(TEXT("HeatBeatTimeOutTime=%f"), ConfigInfo.HeatBeatTimeOutTime));
		FFileHelper::SaveStringArrayToFile(Content, *InPath);
	}
}
