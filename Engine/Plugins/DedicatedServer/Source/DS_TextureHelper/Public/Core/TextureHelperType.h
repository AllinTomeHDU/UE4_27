#pragma once

#include "CoreMinimal.h"


UENUM(BlueprintType)
enum EImageFormatType
{
	PNG				UMETA(DisplayName = "Png"),
	JPEG			UMETA(DisplayName = "Jpeg"),
	GRAYSCALEJPEG	UMETA(DisplayName = "Gray Scale Jpeg"),
	BMP				UMETA(DisplayName = "Bmp"),
	ICO				UMETA(DisplayName = "Ico"),
	EXR				UMETA(DisplayName = "Exr"),
	ICNS			UMETA(DisplayName = "Icns"),
};

UENUM(BlueprintType)
enum EFormatRGB
{
	RGBA UMETA(DisplayName = "RGBA"),
	BGRA UMETA(DisplayName = "BGRA"),
	GRAY UMETA(DisplayName = "Gray"),
};
