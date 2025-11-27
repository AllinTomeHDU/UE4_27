#pragma once

#include "CoreMinimal.h"
#include "NetChannelType.h"
#include "../Channel/NetChannelBase.h"
#include "../Stream/NetChannelIOStream.h"


class DEDICATEDSERVERUTILS_API FRecursionMessageInfo
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
template<> class DEDICATEDSERVERUTILS_API FNetChannelProtocols<ProtocolsNumber>	\
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
#define NETCHANNEL_PROTOCOLS_SEND(InProtocols,...) FNetChannelProtocols<InProtocols>::Send(Channel, __VA_ARGS__);
#define NETCHANNEL_PROTOCOLS_RECV(InProtocols,...) FNetChannelProtocols<InProtocols>::Recv(Channel, __VA_ARGS__);


/**
* 定义协议
*/
DEFINITION_NETCHANNEL_PROTOCOLS(Debug,0)
DEFINITION_NETCHANNEL_PROTOCOLS(Hello,1)
DEFINITION_NETCHANNEL_PROTOCOLS(Challenge,2)
DEFINITION_NETCHANNEL_PROTOCOLS(Login,3)
DEFINITION_NETCHANNEL_PROTOCOLS(Welcom,4)
DEFINITION_NETCHANNEL_PROTOCOLS(Join,5)

