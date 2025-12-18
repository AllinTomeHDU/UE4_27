#pragma once

#include "CoreMinimal.h"
#include "Base/ServerThreadTemplate.h"
#include "../Interface/ServerThreadManagerInterface.h"


#ifdef PLATFORM_PROJECT
/**
* 统一管理异步资源加载，绑定加载完成事件
*/
struct DS_THREADPOOL_API FResourceLoadingManage : FServerThreadTemplateBase<IStreamableContainer, TSharedPtr<struct FStreamableHandle>>
{
	typedef FThreadTemplateBase<IStreamableContainer,TSharedPtr<struct FStreamableHandle>> Super;

	FResourceLoadingManage() : Super(), StreamableManager(new FStreamableManager())
	{}

	~FResourceLoadingManage()
	{
		delete StreamableManager;
	}

protected:
	virtual void SetObjectPath(const TArray<FSoftObjectPath>& InObjectPath) final
	{
		SoftObjectPath = InObjectPath;
	}

	virtual TArray<FSoftObjectPath>& GetObjectPath() final
	{
		return SoftObjectPath;
	}

	virtual FStreamableManager* GetStreamableManager() final
	{
		return StreamableManager;
	}

private:
	TArray<FSoftObjectPath> SoftObjectPath;
	FStreamableManager* StreamableManager;
};
#endif