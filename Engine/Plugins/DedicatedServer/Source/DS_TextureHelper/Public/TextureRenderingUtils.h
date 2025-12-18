#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"

class UTexture2D;

DECLARE_DELEGATE_ThreeParams(FOnScreenShotCaptured, const TArray<FColor>&, int32, int32);


namespace DS_TextureHelper
{
	struct FScreenShot
	{
		FScreenShot(int32 InWidth, int32 InHeight, FOnScreenShotCaptured InOnTestScreenshotCaptured,
					int32 InImageQuality = 80, bool bInShowUI = false, bool bAddFilenameSuffix = true);

	protected:
		void OnScreenshotCapturedInternal(int32 SrcWidth, int32 SrcHeight, const TArray<FColor>& OrigBitmap);
	
	private:
		FOnScreenShotCaptured OnTestScreenshotCaptured;
		FDelegateHandle ScreenShotDelegateHandle;
		int32 ScaledWidth;
		int32 ScaledHeight;
		int32 ImageQuality;
		UObject* Outer;
	};
}