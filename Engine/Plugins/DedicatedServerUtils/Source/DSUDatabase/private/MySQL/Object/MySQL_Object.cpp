#include "MySQL/Object/MySQL_Object.h"
#include "MySQL/Link/MySQL_Link.h"


bool UMySQL_Object::CreateLink(const FMySQLConnectConfig& Config)
{
	Link = MakeShareable(new FMySQL_Link(
		Config.Host,
		Config.User,
		Config.Password,
		Config.Port,
		Config.UnixSocket,
		Config.ClientFlags
	));
	if (!Link) return false;
	if (!Config.Database.IsEmpty())
	{
		return Link->SelectDatabase(Config.Database);
	}
	return true;
}
