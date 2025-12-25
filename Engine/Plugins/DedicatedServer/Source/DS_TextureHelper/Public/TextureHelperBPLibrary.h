#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Definition/TextureHelperType.h"
#include "TextureHelperBPLibrary.generated.h"

class UTexture2D;


UCLASS()
class DS_TEXTUREHELPER_API UTextureHelperBPLibrary : public UObject
{
	GENERATED_BODY()

	//从磁盘里面读取文件转为Texture2D
	UFUNCTION(BlueprintCallable, Category = "TextureHelper")
	static UTexture2D* Uint8ArrayToTexture2DFromDisk(
		const FString& InLoadPath, 
		const EImageFormatType InFormat = EImageFormatType::JPEG, 
		const EFormatRGB InRGBFormat = EFormatRGB::BGRA, 
		bool bTurnBlack = false
	);

	//将数据存储
	UFUNCTION(BlueprintCallable, Category = "TextureHelper")
	static UTexture2D* Uint8ArrayToTexture2D(
		const TArray<uint8>& InData, 
		const EImageFormatType InFormat = EImageFormatType::JPEG, 
		const EFormatRGB InRGBFormat = EFormatRGB::BGRA, 
		bool bTurnBlack = false
	);

	//截屏并且存储在磁盘
	UFUNCTION(BlueprintCallable, meta = (Category = "TextureHelper", WorldContext = WorldContextObject))
	static bool ScreenShotToDisk(
		UObject* WorldContextObject, 
		const FString& InSavePath, 
		bool bShowUI = false, 
		const EImageFormatType InFormat = EImageFormatType::JPEG, 
		const EFormatRGB InRGBFormat = EFormatRGB::BGRA
	);

	//截图 获取到实际截图RGBA数据 这个接口在编辑器里面可以截图，但是Runtime下有问题
	UFUNCTION(BlueprintCallable, meta = (Category = "TextureHelper", WorldContext = WorldContextObject))
	static FVector2D ScreenShot(UObject* WorldContextObject, TArray<FColor>& OutScreenShotData);

	//获取视口尺寸
	UFUNCTION(BlueprintCallable, meta = (Category = "TextureHelper", WorldContext = WorldContextObject))
	static FVector2D GetViewportSize(UObject* WorldContextObject);
};