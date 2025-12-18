#include "TextureHelperBPLibrary.h"
#include "TextureRenderingUtils.h"
#include "Engine/Texture2D.h"
#include "Engine/GameViewportClient.h"
#include "Slate/SceneViewport.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Misc/FileHelper.h"
using namespace DS_TextureHelper;


UTexture2D* UTextureHelperBPLibrary::Uint8ArrayToTexture2DFromDisk(const FString& InLoadPath, const EImageFormatType InFormat, const EFormatRGB InRGBFormat, bool bTurnBlack)
{
	TArray<uint8> ImageData;
	FFileHelper::LoadFileToArray(ImageData, *InLoadPath);
	return Uint8ArrayToTexture2D(ImageData, InFormat, InRGBFormat, bTurnBlack);
}

UTexture2D* UTextureHelperBPLibrary::Uint8ArrayToTexture2D(const TArray<uint8>& InData, const EImageFormatType InFormat, const EFormatRGB InRGBFormat, bool bTurnBlack)
{
	UTexture2D* Texture = nullptr;
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");

	auto WrapperPtr = ImageWrapperModule.CreateImageWrapper((EImageFormat)InFormat);
	if (!WrapperPtr.IsValid()) return nullptr;
	if (!WrapperPtr->SetCompressed(InData.GetData(), InData.Num())) return nullptr;

	TArray<uint8> UncompressedBGRA;
	if (WrapperPtr->GetRaw((ERGBFormat)InRGBFormat,8, UncompressedBGRA))
	{
		if (bTurnBlack)
		{
			for (int32 i = 0 ;i < UncompressedBGRA.Num() ;i+=4)
			{
				if (UncompressedBGRA[i] >= 0 && UncompressedBGRA[i] <= 5 &&
					UncompressedBGRA[i + 1] >= 0 && UncompressedBGRA[i + 1] <= 5 &&
					UncompressedBGRA[i + 2] >= 0 && UncompressedBGRA[i + 2] <= 5)
				{
					UncompressedBGRA[i] = 255 - UncompressedBGRA[i];
					UncompressedBGRA[i + 1] = 255 - UncompressedBGRA[i + 1];
					UncompressedBGRA[i + 2] = 255 - UncompressedBGRA[i + 2];
				}
			}
		}
		Texture = UTexture2D::CreateTransient(WrapperPtr->GetWidth(), WrapperPtr->GetHeight(),PF_B8G8R8A8);
		void *TextureAddr = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(TextureAddr, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
		Texture->PlatformData->Mips[0].BulkData.Unlock();
		Texture->UpdateResource();
	}
	return Texture;
}

bool UTextureHelperBPLibrary::ScreenShotToDisk(UObject* WorldContextObject, const FString& InSavePath, bool bShowUI, const EImageFormatType InFormat, const EFormatRGB InRGBFormat)
{
	auto TextureHelperFunc = [](const TArray<FColor>& InRawData, const FVector2D& InViewportSize, const FString& InSavePath,
								const EImageFormatType InFormat, const EFormatRGB InRGBFormat)->bool
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

		TSharedPtr<IImageWrapper> WrapperPtr = ImageWrapperModule.CreateImageWrapper((EImageFormat)InFormat);
		WrapperPtr->SetRaw(InRawData.GetData(), InRawData.GetAllocatedSize(), InViewportSize.X, InViewportSize.Y, (ERGBFormat)InRGBFormat, 8);

		const TArray64<uint8>& IMGData = WrapperPtr->GetCompressed();
		return FFileHelper::SaveArrayToFile(IMGData, *InSavePath);
	};

	auto HandleLambda = FOnScreenShotCaptured::CreateLambda(
		[TextureHelperFunc, InSavePath, InFormat, InRGBFormat] 
		(const TArray<FColor>& InData, int32 InX, int32 InY)
		{
			TextureHelperFunc(InData, FVector2D(InX, InY), InSavePath, InFormat, InRGBFormat);
		}
	);

	FVector2D ViewportSize = GetViewportSize(WorldContextObject);
	return (new FScreenShot(ViewportSize.X, ViewportSize.Y, HandleLambda, 90, bShowUI)) != nullptr;
}

FVector2D UTextureHelperBPLibrary::ScreenShot(UObject* WorldContextObject, TArray<FColor>& OutScreenShotData)
{
	FVector2D ViewportSize = FVector2D::ZeroVector;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UGameViewportClient* InViewportClient = World->GetGameViewport())
		{
			if (FSceneViewport* InViewport = InViewportClient->GetGameViewport())
			{
				InViewportClient->GetViewportSize(ViewportSize);

				if (GetViewportScreenShot(InViewport->GetViewport(), OutScreenShotData, FIntRect(0, 0, ViewportSize.X, ViewportSize.Y)))
				{
					//成功截屏
					UE_LOG(LogTemp, Display, TEXT("ScreenShot Success"));
				}
			}
		}
	}
	return ViewportSize;
}

FVector2D UTextureHelperBPLibrary::GetViewportSize(UObject* WorldContextObject)
{
	FVector2D ViewportSize = FVector2D::ZeroVector;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UGameViewportClient* InViewportClient = World->GetGameViewport())
		{
			if (FSceneViewport* InViewport = InViewportClient->GetGameViewport())
			{
				InViewportClient->GetViewportSize(ViewportSize);
			}
		}
	}
	return ViewportSize;
}
