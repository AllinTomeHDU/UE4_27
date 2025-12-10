#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Misc/FileHelper.h"
#include "FileHelperBPLibrary.generated.h"


UENUM(BlueprintType)
enum class EHashOptionsBP : uint8
{
	None = 0,
	EnableVerify,		// 启用异步任务，加载时哈希校验
	ErrorMissingHash,	// 若缺少哈希记录就报错
};

UENUM(BlueprintType)
enum class EEncodingOptionsBP : uint8
{
	AutoDetect,
	ForceAnsi,
	ForceUnicode,
	ForceUTF8,
	ForceUTF8WithoutBOM
};

namespace DSUFileHelper
{
	FFileHelper::EHashOptions ToHashOptions(EHashOptionsBP HashOptions);
}

UCLASS()
class DSUFILEHELPER_API UFileHelperBPLibrary : public UObject
{
	GENERATED_BODY()

public:
	/**
	* 找到某个文件夹中最近修改过的文件，可指定后缀名。
	* @param bRecurse    是否递归查找子文件夹
	* @param InSuffix    需要过滤的文件后缀，","隔开
	*/
	UFUNCTION(BlueprintCallable, Category = "DedicatedServerUtil|FileHelper")
	static bool GetRecentlyModifiedFile(FString& OutPath, const FString& FilePath, bool bRecurse = true, const FString& InSuffix = TEXT(""));

	/**
	* 从磁盘读取一个文件（比如 .uasset, .png, .bin 等）到一个字节数组 TArray<uint8> 中，
	* 并在文件末尾添加两个空字节（0x00 0x00）用于安全缓冲。
	*/
	UFUNCTION(BlueprintCallable, Category = "DedicatedServerUtil|FileHelper")
	static bool LoadFileToArray(TArray<uint8>& Result, const FString& Filename, int32 Flags = 0);

	/**
	* 从指定文件路径中读取文本文件内容到 FString，自动识别编码格式（UTF-8 / UTF-16 / ANSI 等），
	* 并根据传入的 VerifyFlags 参数决定是否进行文件哈希校验。
	*/
	UFUNCTION(BlueprintCallable, Category = "DedicatedServerUtil|FileHelper")
	static bool LoadFileToString(FString& Result, const FString& Filename, EHashOptionsBP VerifyFlags);

	/**
	* 读取一个文本文件的全部内容，并按“行”分割，每一行作为一个 FString 元素存入 Result 数组中。
	* 自动识别文件编码（ANSI、UTF-8、Unicode），并根据 VerifyFlags 参数决定是否启用哈希验证。
	*/
	UFUNCTION(BlueprintCallable, Category = "DedicatedServerUtil|FileHelper")
	static bool LoadFileToStringArray(TArray<FString>& Result, const FString& Filename);

	/**
	* 将二进制数组保存进文件。
	*/
	UFUNCTION(BlueprintCallable, Category = "DedicatedServerUtil|FileHelper")
	static bool SaveArrayToFile(const TArray<uint8>& Array, const FString& Filename, int32 WriteFlags = 0);

	/**
	* 将字符串写入文件，支持所有编码格式，
	* 能自动识别各种编码格式，并在所有主要平台（Windows、Linux、macOS 等）上都能正确工作。
	*/
	UFUNCTION(BlueprintCallable, Category = "DedicatedServerUtil|FileHelper")
	static bool SaveStringToFile(const FString& String, const FString& Filename, EEncodingOptionsBP EncodingOptions = EEncodingOptionsBP::AutoDetect, int32 WriteFlags = 0);

	/**
	* 将一组 FString（通常表示一行一行的文本）写入到指定文件中，
	* 自动处理文件编码（ANSI / UTF-8 / UTF-16）并支持不同平台，
	* 通过 WriteFlags 控制写入行为（例如是否追加、是否静默等）。
	*/
	UFUNCTION(BlueprintCallable, Category = "DedicatedServerUtil|FileHelper")
	static bool SaveStringArrayToFile(const TArray<FString>& Lines, const FString& Filename, EEncodingOptionsBP EncodingOptions = EEncodingOptionsBP::AutoDetect, int32 WriteFlags = 0);

	/**
	* 生成一个新的、不与现有文件冲突的文件名，主要用于保存截图、导出图片、写缓存文件等场景。
	* @param Pattern		带路径文件名
	* @param Extension		文件后缀
	*/
	UFUNCTION(BlueprintCallable, Category = "DedicatedServerUtil|FileHelper")
	static bool GenerateNextBitmapFilename(const FString& Pattern, const FString& Extension, FString& OutFilename);

	/**
	 * 将一块颜色数据（FColor数组）保存成一张 BMP 图片文件，支持 24位RGB 和 32位RGBA 位图。
	 * @param Pattern		 带路径文件名，如不带扩展名会自动加编号，例如 C:/Shots/Screen → Screen0001.bmp
	 * @param DataWidth		 图像的宽度，像素 > 0
	 * @param DataHeight	 图像的高度，像素 > 0
	 * @param Data			 原始颜色数据的首地址
	 * @param SubRectangle	 选定要保存的子区域，若忽略则保存整张图片
	 * @param bInWriteAlpha  是否写入 Alpha 通道，false->24位RGB，true->32为BMP
	 */
	UFUNCTION(BlueprintCallable, Category = "DedicatedServerUtil|FileHelper")
	static bool CreateBitmap(const FString& Pattern, int32 DataWidth, int32 DataHeight, const FColor& Data, FVector4 SubRectangle, FString& OutFilename, bool bInWriteAlpha = false);

	/**
	* 从一个 ANSI编码的文本文件 读取内容，并将每一行转换为 FString，按顺序放入 OutStrings 数组中。
	*/
	UFUNCTION(BlueprintCallable, Category = "DedicatedServerUtil|FileHelper")
	static bool LoadANSITextFileToStrings(const FString& InFilename, TArray<FString>& OutStrings);

	/**
	* 检查一个文件名是否适合保存文件，会验证：
	*	1.文件路径长度是否超出平台允许的最大值；
	*	2.文件名中是否包含非法字符；
	*	3.路径是否为空；
	*	4.其他平台相关限制。
	*/
	UFUNCTION(BlueprintCallable, Category = "DedicatedServerUtil|FileHelper")
	static bool IsFilenameValidForSaving(const FString& Filename, FText& OutError);
};