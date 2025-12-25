#pragma once

#include "CoreMinimal.h"
#include "NetChannelType.h"
#include "../NetChannelManager.h"
#include "../Channel/NetChannelBase.h"
#include "../Stream/NetChannelIOStream.h"


class DS_NETCHANNEL_API FRecursionMessageInfo
{
public:
	template<typename ...ParamTypes>
	static int32 GetParamsNum(ParamTypes& ...Params)
	{
		return sizeof...(Params);
	}

	template<typename ...ParamTypes>
	static void ConstructSendParams(FNetChannelIOStream& InStream, ParamTypes& ...Params) {}

	template<class T, typename ...ParamTypes>
	static void ConstructSendParams(FNetChannelIOStream& InStream, T& FirstParam, ParamTypes& ...Params)
	{
		InStream << FirstParam;
		ConstructSendParams(InStream, Params...);
	}

	template<typename ...ParamTypes>
	static void ConstructRecvParams(FNetChannelIOStream& InStream, ParamTypes& ...Params) {}

	template<class T, typename ...ParamTypes>
	static void ConstructRecvParams(FNetChannelIOStream& InStream, T& FirstParam, ParamTypes& ...Params)
	{
		InStream >> FirstParam;
		ConstructRecvParams(InStream, Params...);
	}
};

template<uint32 ProtocolsType>
class FNetChannelProtocols {};


#define DEFINITION_NETCHANNEL_BUFFER \
TArray<uint8> BUFFER; \
FNetChannelIOStream STREAM(BUFFER);

#define DEFINITION_NETCHANNEL_HEAD(ProtocolsName) \
FNetBunchHead HEAD; \
HEAD.ProtocolsNumber = (uint32)P_##ProtocolsName; \
HEAD.ChannelGUID = InChannel->GetGUID(); \
STREAM << HEAD;


#define DEFINITION_NETCHANNEL_PROTOCOLS(ProtocolsName,ProtocolsNumber) \
enum{P_##ProtocolsName = ProtocolsNumber}; \
template<> class DS_NETCHANNEL_API FNetChannelProtocols<ProtocolsNumber>	\
{ \
public: \
	template<typename ...ParamTypes> \
	static void Send(FNetChannelBase* InChannel, ParamTypes& ...Params) \
	{ \
		DEFINITION_NETCHANNEL_BUFFER \
		DEFINITION_NETCHANNEL_HEAD(ProtocolsName) \
		FRecursionMessageInfo::ConstructSendParams(STREAM, Params...); \
		InChannel->Send(BUFFER); \
	} \
	template<typename ...ParamTypes> \
	static void Recv(FNetChannelBase* InChannel, ParamTypes& ...Params) \
	{ \
		DEFINITION_NETCHANNEL_BUFFER \
		if (InChannel->Recv(BUFFER)) \
		{ \
			STREAM.Seek(sizeof(FNetBunchHead)); \
			FRecursionMessageInfo::ConstructRecvParams(STREAM, Params...); \
		} \
	} \
};


/**
* 调用的时候需要访问权限内有一个名为 Channel 的变量
*/
#define NETCHANNEL_PROTOCOLS_SEND(InProtocols,...) FNetChannelProtocols<InProtocols>::Send(Channel, ##__VA_ARGS__);
#define NETCHANNEL_PROTOCOLS_RECV(InProtocols,...) FNetChannelProtocols<InProtocols>::Recv(Channel, ##__VA_ARGS__);
/*
* GCC/Clang的可变参数写法：args... 与 ##args
*/


#define CLIENT_SEND(ClientManager,InProtocols,...) \
if (ClientManager && ClientManager->GetController()) \
{ \
	if (auto SENDCHANNEL = ClientManager->GetLocalChannel()) \
	{ \
		FNetChannelProtocols<InProtocols>::Send(SENDCHANNEL,##__VA_ARGS__); \
	} \
}
#define CLIENT_RECV(ClientManager,InProtocols,...) \
if (ClientManager && ClientManager->GetController()) \
{ \
	if (auto RECVCHANNEL = ClientManager->GetLocalChannel()) \
	{ \
		FNetChannelProtocols<InProtocols>::Recv(RECVCHANNEL,##__VA_ARGS__); \
	} \
}

#define SERVER_SEND(ServerManager,AddrInfo,InProtocols,...) \
if (ServerManager && ServerManager->GetController()) \
{ \
	if (auto SENDCHANNEL = ServerManager->GetRemoteChannel(AddrInfo)) \
	{ \
		FNetChannelProtocols<InProtocols>::Send(SENDCHANNEL,##__VA_ARGS__); \
	} \
}
#define SERVER_RECV(ServerManager,AddrInfo,InProtocols,...) \
if (ServerManager && ServerManager->GetController()) \
{ \
	if (auto RECVCHANNEL = ServerManager->GetRemoteChannel(AddrInfo)) \
	{ \
		FNetChannelProtocols<InProtocols>::Recv(RECVCHANNEL,##__VA_ARGS__); \
	} \
}



/**
* 建立连接（三次握手）协议
*/
DEFINITION_NETCHANNEL_PROTOCOLS(Debug,10000)		// server/client
DEFINITION_NETCHANNEL_PROTOCOLS(Hello,10001)		// client
DEFINITION_NETCHANNEL_PROTOCOLS(Welcom,10002)		// server
DEFINITION_NETCHANNEL_PROTOCOLS(Join,10003)			// client
DEFINITION_NETCHANNEL_PROTOCOLS(Upgrade,10004)		// server
DEFINITION_NETCHANNEL_PROTOCOLS(Close,100005)		// client
DEFINITION_NETCHANNEL_PROTOCOLS(HeartBeat,10006)	// client

/*
* 报错协议
*/
DEFINITION_NETCHANNEL_PROTOCOLS(Failure,10010)			// server/client
DEFINITION_NETCHANNEL_PROTOCOLS(ServerFailure,10011)	// Server
DEFINITION_NETCHANNEL_PROTOCOLS(ClientFailure,10012)	// Client

/*
* 注册协议
*/
DEFINITION_NETCHANNEL_PROTOCOLS(Register,10020)				// Client
DEFINITION_NETCHANNEL_PROTOCOLS(RegisterSuccess,10021)		// Server
DEFINITION_NETCHANNEL_PROTOCOLS(RegisterFailure, 10022)		// Server
DEFINITION_NETCHANNEL_PROTOCOLS(AccountAlreadyExits,10023)	// Server，账号已存在

/*
* 登录协议
*/
DEFINITION_NETCHANNEL_PROTOCOLS(Login,10030)				// Client
DEFINITION_NETCHANNEL_PROTOCOLS(LoginSuccess,10031)			// Server
DEFINITION_NETCHANNEL_PROTOCOLS(LoginFailure,10032)			// Server
DEFINITION_NETCHANNEL_PROTOCOLS(AbsentAccount,10033)		// Server，账号不存在
DEFINITION_NETCHANNEL_PROTOCOLS(AbnormalAccount,10034)		// Server，账号异常
DEFINITION_NETCHANNEL_PROTOCOLS(IncorrectPassword, 10035)	// Server，密码错误
DEFINITION_NETCHANNEL_PROTOCOLS(VerificationError,10036)	// Server，验证错误

/*
* 大厅服务器协议
*/
DEFINITION_NETCHANNEL_PROTOCOLS(RequestUserAssets,10040)		// Client
DEFINITION_NETCHANNEL_PROTOCOLS(ResponseUserAssets,10041)		// Server
DEFINITION_NETCHANNEL_PROTOCOLS(FailedRequestUserAssets,10042)	// Server
