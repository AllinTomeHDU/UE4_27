#include "FileHelperBPLibrary.h"


FFileHelper::EHashOptions DedicatedServerUtil::ToHashOptions(EHashOptionsBP HashOptions)
{
	switch (HashOptions)
	{
	case EHashOptionsBP::EnableVerify:
		return FFileHelper::EHashOptions::EnableVerify;
	case EHashOptionsBP::ErrorMissingHash:
		return FFileHelper::EHashOptions::ErrorMissingHash;
	}
	return FFileHelper::EHashOptions::None;
}

bool UFileHelperBPLibrary::GetRecentlyModifiedFile(FString& OutPath, const FString& FilePath, bool bRecurse, const FString& InSuffix)
{
	OutPath.Empty();

	// 分割后缀
	TArray<FString> Suffixs;
	InSuffix.ParseIntoArray(Suffixs, TEXT(","));

	// 判定是否存在后缀的内容
	auto IsSuffix = [&](const FString& InString) -> bool
		{
			if (Suffixs.Num() == 0) return true;
			for (auto& Item : Suffixs)
			{
				if (InString.EndsWith(Item)) return true;
			}
			return false;
		};

	// 获取目录下所有文件
	TArray<FString> Filenames;
	bRecurse ? IFileManager::Get().FindFilesRecursive(Filenames, *FilePath, TEXT("*.*"), true, false)
		: IFileManager::Get().FindFiles(Filenames, *FilePath, true, false);

	FDateTime DateTime(0);
	for (auto& Item : Filenames)
	{
		if (IsSuffix(Item))
		{
			FDateTime TmpDateTime = IFileManager::Get().GetTimeStamp(*Item);
			if (TmpDateTime > DateTime)
			{
				DateTime = TmpDateTime;
				OutPath = Item;
			}
		}
	}

	return Filenames.Num() > 0 && !OutPath.IsEmpty();
}

bool UFileHelperBPLibrary::LoadFileToArray(TArray<uint8>& Result, const FString& Filename, int32 Flags)
{
	return FFileHelper::LoadFileToArray(Result, *Filename, Flags);
}

bool UFileHelperBPLibrary::LoadFileToString(FString& Result, const FString& Filename, EHashOptionsBP VerifyFlags)
{
	return FFileHelper::LoadFileToString(Result, *Filename, DedicatedServerUtil::ToHashOptions(VerifyFlags));
}

bool UFileHelperBPLibrary::LoadFileToStringArray(TArray<FString>& Result, const FString& Filename)
{
	return FFileHelper::LoadFileToStringArray(Result, *Filename);
}

bool UFileHelperBPLibrary::SaveArrayToFile(const TArray<uint8>& Array, const FString& Filename, int32 WriteFlags)
{
	return FFileHelper::SaveArrayToFile(Array, *Filename, &IFileManager::Get(), WriteFlags);
}

bool UFileHelperBPLibrary::SaveStringToFile(const FString& String, const FString& Filename, EEncodingOptionsBP EncodingOptions, int32 WriteFlags)
{
	return FFileHelper::SaveStringToFile(String, *Filename, (FFileHelper::EEncodingOptions)EncodingOptions, &IFileManager::Get(), WriteFlags);
}

bool UFileHelperBPLibrary::SaveStringArrayToFile(const TArray<FString>& Lines, const FString& Filename, EEncodingOptionsBP EncodingOptions, int32 WriteFlags)
{
	return FFileHelper::SaveStringArrayToFile(Lines, *Filename, (FFileHelper::EEncodingOptions)EncodingOptions, &IFileManager::Get(), WriteFlags);
}

bool UFileHelperBPLibrary::GenerateNextBitmapFilename(const FString& Pattern, const FString& Extension, FString& OutFilename)
{
	return FFileHelper::GenerateNextBitmapFilename(*Pattern, Extension, OutFilename);
}

bool UFileHelperBPLibrary::CreateBitmap(const FString& Pattern, int32 DataWidth, int32 DataHeight, const FColor& Data, FVector4 SubRectangle, FString& OutFilename, bool bInWriteAlpha)
{
	TSharedPtr<FIntRect> Rect = nullptr;
	if (!SubRectangle.IsNearlyZero3(0))
	{
		Rect = MakeShareable(new FIntRect);
		Rect->Min.X = SubRectangle.X;
		Rect->Min.Y = SubRectangle.Y;
		Rect->Max.X = SubRectangle.Z;
		Rect->Max.Y = SubRectangle.W;
	}

	return FFileHelper::CreateBitmap(
		*Pattern,
		DataWidth,
		DataHeight,
		&Data,
		Rect.Get(),
		&IFileManager::Get(),
		(OutFilename.IsEmpty() ? NULL : &OutFilename),
		bInWriteAlpha
	);
}

bool UFileHelperBPLibrary::LoadANSITextFileToStrings(const FString& InFilename, TArray<FString>& OutStrings)
{
	return FFileHelper::LoadANSITextFileToStrings(*InFilename, &IFileManager::Get(), OutStrings);
}

bool UFileHelperBPLibrary::IsFilenameValidForSaving(const FString& Filename, FText& OutError)
{
	return FFileHelper::IsFilenameValidForSaving(Filename, OutError);
}
