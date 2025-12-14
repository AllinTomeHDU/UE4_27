#include "MySQL/Object/MySQL_Object.h"
#include "MySQL/Link/MySQL_Link.h"


bool UMySQL_Object::CreateLink(const FMySQLConnectConfig& Config)
{
	Link = MakeShareable(new FMySQL_Link(
		Config.Host,
		Config.User,
		Config.Password,
		Config.Port,
		Config.Database,
		Config.UnixSocket,
		Config.ClientFlags
	));
	return Link != nullptr;
}
