#pragma once

#include "CoreMinimal.h"
#include "Base/ServerThreadTemplate.h"
#include "../Interface/ServerThreadManagerInterface.h"


/**
* 中间封装层：定义一个以 IAbandonableContainer 为执行容器的线程管理器
*	- 绑定同步绑定将阻止启动线程，并在完成任务后将其激活
*	- /创建异步绑定直接开始，并在任务完成后自动销毁
*/
struct DS_THREADPOOL_API FThreadAbandonableManager
	: public FServerThreadTemplate<IThreadAbandonableContainer>
{

};