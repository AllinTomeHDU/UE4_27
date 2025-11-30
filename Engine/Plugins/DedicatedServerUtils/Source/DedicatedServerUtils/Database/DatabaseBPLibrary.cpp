#include "DatabaseBPLibrary.h"

#ifdef TEXT
#undef TEXT
#endif
#include "MySQL/mysql.h"


void UDatabaseBPLibrary::TestMySQL_1()
{
	/**
	* 初始化
	*/
	MYSQL mysql;
	mysql_library_init(0, NULL, NULL);  // 保证线程安全，只分配一次内存
	mysql_init(&mysql);  // 分配内存

	/**
	* 连接配置：在建立连接前，配置连接行为或环境参数。
	*/
	int is_reconnect = 1;
	int opt_res = mysql_options(&mysql, MYSQL_OPT_RECONNECT, &is_reconnect);
	int buffer_size = 1024 * 1024 * 1024;
	opt_res = mysql_options(&mysql, MYSQL_OPT_MAX_ALLOWED_PACKET, &buffer_size) ? 1 : opt_res;
	if (opt_res)
	{
		UE_LOG(LogTemp, Error, TEXT("mysql_options failed..."));
	}


	/**
	* 连接数据库：32个bit位，从 1 到 1<<32
	*/
	{
		//CLIENT_LONG_PASSWORD;    // 支持旧密码认证协议。
		CLIENT_FOUND_ROWS;         // 返回找到的行数而不是受影响的行数。
		//CLIENT_LONG_FLAG;        // 支持更长的列标志。
		//CLIENT_CONNECT_WITH_DB;  // 支持在连接时指定数据库。
		CLIENT_NO_SCHEMA;		   // 不允许使用 database.table.column 语法。
		CLIENT_COMPRESS;		   // 支持压缩协议。
		CLIENT_ODBC;			   // 支持 ODBC 行为。
		CLIENT_LOCAL_FILES;		   // 支持 LOAD DATA LOCAL，支持批量插入。
		CLIENT_IGNORE_SPACE;	   // 忽略函数名后的空格。
		//CLIENT_PROTOCOL_41;	   // 支持 4.1 协议。
		CLIENT_INTERACTIVE;		   // 客户端是交互式的，增量超时控制。
		CLIENT_SSL;				   // 支持 SSL 加密。
		//CLIENT_IGNORE_SIGPIPE;   // 忽略网络故障时的 SIGPIPE 信号。
		//CLIENT_TRANSACTIONS;	   // 支持事务。
		//CLIENT_RESERVED;		   // DEPRECATED: Old flag for 4.1 protocol
		//CLIENT_RESERVED2;		   // DEPRECATED: Old flag for 4.1 authentication CLIENT_SECURE_CONNECTION
		CLIENT_MULTI_STATEMENTS;   // 支持单字符串多语句。
		CLIENT_MULTI_RESULTS;	   // 支持多结果集。
		//CLIENT_PS_MULTI_RESULTS; // 支持多结果集和OUT参数，但仅适用于预处理语句协议。
		//CLIENT_PLUGIN_AUTH;	   // 支持插件认证。
		//CLIENT_CONNECT_ATTRS;	   // 支持在握手阶段发送连接属性。
		//CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA;	// 启用对认证响应数据包长度超过 255 字节的支持。
		//CLIENT_CAN_HANDLE_EXPIRED_PASSWORDS;	    // 支持处理过期密码的扩展。
		//CLIENT_SESSION_TRACK;    // 支持会话状态跟踪。
		//CLIENT_DEPRECATE_EOF;	   // 不再需要 EOF 包。
		//CLIENT_OPTIONAL_RESULTSET_METADATA;  // 允许服务器在返回结果集时省略某些元数据，从而减少数据传输量。
		//CLIENT_SSL_VERIFY_SERVER_CERT;	   // 支持验证服务器证书。
		//CLIENT_REMEMBER_OPTIONS;			   // 支持在连接失败后保留选项。
	}
	const char* host = "127.0.0.1";
	const char* user = "root";
	const char* pwd = "20212526Hdu.";
	const char* db = "test";
	const char* table = "playerinfo";
	const uint32 port = 3306;
	// mysql_real_connect(&mysql, host, user, pwd, db, port, 0, CLIENT_COMPRESS|CLIENT_MULTI_STATEMENTS);
	if (mysql_real_connect(&mysql, host, user, pwd, db, port, 0, 0))
	{
		/**
		* 检测是否连接成功
		*/
		int connect_res = mysql_ping(&mysql);
		if (connect_res == 0)
		{
			UE_LOG(LogTemp, Display, TEXT("mysql_real_connect successful!"));
		}
		else
		{
			/**
			* MySQL 报错字段
			*/
			unsigned int error_number = mysql_errno(&mysql);
			{
				//  1005 : 创建表失败
				//	1006 : 创建数据库失败
				//	1007 : 数据库已存在，创建数据库失败
				//	1008 : 数据库不存在，删除数据库失败
				//	1009 : 不能删除数据库文件导致删除数据库失败
				//	1010 : 不能删除数据目录导致删除数据库失败
				//	1011 : 删除数据库文件失败
				//	1012 : 不能读取系统表中的记录
				//	1016 : 无法打开文件
				//	1020 : 记录已被其他用户修改
				//	1021 : 硬盘剩余空间不足，请加大硬盘可用空间
				//	1022 : 关键字重复，更改记录失败
				//	1023 : 关闭时发生错误
				//	1024 : 读文件错误
				//	1025 : 更改名字时发生错误
				//	1026 : 写文件错误
				//	1032 : 记录不存在
				//	1036 : 数据表是只读的，不能对它进行修改
				//	1037 : 系统内存不足，请重启数据库或重启服务器
				//	1038 : 用于排序的内存不足，请增大排序缓冲区
				//	1040 : 已到达数据库的最大连接数，请加大数据库可用连接数
				//	1041 : 系统内存不足
				//	1042 : 无效的主机名
				//	1043 : 无效连接
				//	1044 : 当前用户没有访问数据库的权限
				//	1045 : 不能连接数据库，用户名或密码错误
				//	1040 : 最大连接数
				//	1048 : 字段不能为空
				//	1049 : 数据库不存在
				//	1050 : 数据表已存在
				//	1051 : 数据表不存在
				//	1054 : 字段不存在
				//	1065 : 无效的SQL语句，SQL语句为空
				//	1081 : 不能建立Socket连接
				//	1114 : 数据表已满，不能容纳任何记录
				//	1116 : 打开的数据表太多
				//	1129 : 数据库出现异常，请重启数据库
				//	1130 : 连接数据库失败，没有连接数据库的权限
				//	1133 : 数据库用户不存在
				//	1141 : 当前用户无权访问数据库
				//	1142 : 当前用户无权访问数据表
				//	1143 : 当前用户无权访问数据表中的字段
				//	1146 : 数据表不存在
				//	1147 : 未定义用户对数据表的访问权限
				//	1149 : SQL语句语法错误
				//	1158 : 网络错误，出现读错误，请检查网络连接状况
				//	1159 : 网络错误，读超时，请检查网络连接状况
				//	1160 : 网络错误，出现写错误，请检查网络连接状况
				//	1161 : 网络错误，写超时，请检查网络连接状况
				//	1062 : 字段值重复，入库失败
			}
			UE_LOG(LogTemp, Error, TEXT("errir: %d"), error_number);
			return;
		}

		/**
		* 数据表查询
		*/
		MYSQL_RES* result = nullptr;
		int32 result_int = INDEX_NONE;
		char select[] = "SELECT * FROM playerinfo";
		//if ((result_int = mysql_real_query(&mysql, select, strlen(select))) == 0)
		if ((result_int = mysql_query(&mysql, select)) == 0)
		{

			//if ((result = mysql_use_result(&mysql)) != nullptr)	// 在服务器内存直接读
			if ((result = mysql_store_result(&mysql)) != nullptr)	// 存储到本地缓存，再从本地读取
			{
				/**
				* 获取全部字段的所有属性
				*/
				MYSQL_FIELD* mysql_field_arr = mysql_fetch_field(result);

				while (MYSQL_ROW sql_row = mysql_fetch_row(result))
				{
					// 获取字段数量
					//int fields_num = mysql_num_fields(result);
					int fields_num = mysql_field_count(&mysql);	// 可在查询成功之前调用

					unsigned long* len = mysql_fetch_lengths(result);  // 获取此行各字段的字符串长度

					for (int i = 0; i < fields_num; ++i)
					{
						//UE_LOG(LogTemp, Display, TEXT("fieldlen: %ld, field_value: %s"),
						//	len[i], ANSI_TO_TCHAR(sql_row[i]));

						/* 获取某个字段的所有属性 */
						// MYSQL_FIELD* mysql_field = mysql_fetch_field_direct(result, i);
						MYSQL_FIELD* mysql_field = &mysql_field_arr[i];
						{
							UE_LOG(LogTemp, Display, TEXT("name: %s"), ANSI_TO_TCHAR(mysql_field->name));
							UE_LOG(LogTemp, Display, TEXT("org_name: %s"), ANSI_TO_TCHAR(mysql_field->org_name));
							UE_LOG(LogTemp, Display, TEXT("table: %s"), ANSI_TO_TCHAR(mysql_field->table));
							UE_LOG(LogTemp, Display, TEXT("org_table: %s"), ANSI_TO_TCHAR(mysql_field->org_table));
							UE_LOG(LogTemp, Display, TEXT("org_table: %s"), ANSI_TO_TCHAR(mysql_field->org_table));
							UE_LOG(LogTemp, Display, TEXT("db: %s"), ANSI_TO_TCHAR(mysql_field->db));
							UE_LOG(LogTemp, Display, TEXT("catalog: %s"), ANSI_TO_TCHAR(mysql_field->catalog));
							UE_LOG(LogTemp, Display, TEXT("def: %s"), ANSI_TO_TCHAR(mysql_field->def));
							UE_LOG(LogTemp, Display, TEXT("length: %ld"), mysql_field->length);
							UE_LOG(LogTemp, Display, TEXT("max_length: %ld"), mysql_field->max_length);
							UE_LOG(LogTemp, Display, TEXT("name_length: %d"), mysql_field->name_length);
							UE_LOG(LogTemp, Display, TEXT("org_name_length: %d"), mysql_field->org_name_length);
							UE_LOG(LogTemp, Display, TEXT("db_length: %d"), mysql_field->db_length);
							UE_LOG(LogTemp, Display, TEXT("catalog_length: %d"), mysql_field->catalog_length);
							UE_LOG(LogTemp, Display, TEXT("def_length: %d"), mysql_field->def_length);
							UE_LOG(LogTemp, Display, TEXT("flags: %d"), mysql_field->flags);
							UE_LOG(LogTemp, Display, TEXT("decimals: %d"), mysql_field->decimals);
							UE_LOG(LogTemp, Display, TEXT("charsetnr: %d"), mysql_field->charsetnr);
							UE_LOG(LogTemp, Display, TEXT("type: %d"), mysql_field->type);
							UE_LOG(LogTemp, Display, TEXT("----------"));
						}
					}
					UE_LOG(LogTemp, Display, TEXT("\n"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("mysql_store_result failed..."));
				return;
			}
		}

		if (result)
		{
			mysql_free_result(result);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("mysql connect failed..."));
	}

	mysql_close(&mysql);
	mysql_library_end();
}

void UDatabaseBPLibrary::TestMySQL_2()
{
	// 预处理查询：减少服务器负荷，提高服务器响应速度
	MYSQL mysql;
	mysql_library_init(0, NULL, NULL);
	mysql_init(&mysql);

	const char* host = "127.0.0.1";
	const char* user = "root";
	const char* pwd = "20212526Hdu.";
	const char* db = "test";
	const char* table = "playerinfo";
	const uint32 port = 3306;
	if (mysql_real_connect(&mysql, host, user, pwd, db, port, 0, 0))
	{
		MYSQL_STMT* stmt_ptr = mysql_stmt_init(&mysql);
		char* SQL = "INSERT INTO playerinfo VALUES(?,?,?)";	// 防止注入攻击
		if (mysql_stmt_prepare(stmt_ptr, SQL, strlen(SQL)))
		{
			UE_LOG(LogTemp, Error, TEXT("mysql_stmt_prepare error: %s"), ANSI_TO_TCHAR(mysql_stmt_error(stmt_ptr)));
			return;
		}

		/**
		* 绑定数据进行写入
		*/
		MYSQL_BIND params[3];

		char* id = "10003";
		unsigned long id_len = strlen(id);
		params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
		params[0].buffer = id;
		params[0].length = &id_len;
		params[0].is_null = 0;

		char* name = "zhaoxiaohua";
		unsigned long name_len = strlen(name);
		params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
		params[1].buffer = name;
		params[1].length = &name_len;
		params[1].is_null = 0;

		uint8 sex = true;
		params[2].buffer_type = MYSQL_TYPE_TINY;
		params[2].buffer = &sex;
		params[2].length = 0;
		params[2].is_null = 0;

		if (mysql_stmt_bind_param(stmt_ptr, params))
		{
			UE_LOG(LogTemp, Error, TEXT("mysql_stmt_bind_param error: %s"), ANSI_TO_TCHAR(mysql_stmt_error(stmt_ptr)));
			return;
		}

		if (mysql_stmt_execute(stmt_ptr))
		{
			UE_LOG(LogTemp, Error, TEXT("mysql_stmt_execute error: %s"), ANSI_TO_TCHAR(mysql_stmt_error(stmt_ptr)));
			return;
		}
	}

	/**
	* 数据类型
	*/
	{
		// 字符串类型
		//   - char       固定长度，255（定长字符串标识、识别码），支持索引
		//   - varchar    可变长度，255，额外2字节长度信息（短字符串字段），支持索引
		//   - tinytext   可变长度，255，额外1字节长度信息（小段说明），索引需指定长度
		//   - text       可变长度，64KB，额外2字节长度信息（长文本、文章），索引需指定长度
		//   - mediumtext 可变长度，16MB，额外3字节长度信息（日志、大文本），索引需指定长度
		//   - longtext   可变长度，4GB，额外4字节长度信息（超大文本、HTML），索引需指定长度
		//	 :::::::::::: char/varchar 是通用字符串，直接存储在数据行内，效率较高
		//   :::::::::::: blob/text 类型存在独立存储区，数据行内仅存储指针，效率较低
		// 
		// 二进制流储存对象
		//   - tinyblob   255，长度前缀1字节
		//   - blob       64KB，长度前缀2字节
		//   - mediumblob 16MB，长度前缀3字节
		//   - longblob   4GB，长度前缀4字节
		//	 :::::::::::: blob与text的区别就是，一个是纯二进制数据，一个是文本数据
		// 
		// 数字类型
		//   - tinyint     1字节，bool类型
		//   - smallint    2字节
		//   - mediumint   3字节
		//   - int/integer 4字节
		//   - bigint      8字节
		//	 ::::::::::::: 所有上述类型都可以加上 UNSIGNED 修饰表示无符号
		//   - float       4字节
		//   - double/real 8字节
		//   - decimal(M,D)/numeric(M,D) M=总位数，D=小数位，字符串方式存储，不会有二进制舍入误差
		// 
		// 时间类型
		//   - date      3字节
		//	 - time      3~5字节，可精确到微秒
		//   - datetime  5~8字节，无时区
		//   - timestamp 4~7字节，有时区，UTC时间戳，按时区转换后的本地时间
		//   - year      1字节
		//   ::::::::::: 格式通常为 YYYY-MM-DD HH:MM:SS
	}

	mysql_close(&mysql);
	mysql_library_end();
}


