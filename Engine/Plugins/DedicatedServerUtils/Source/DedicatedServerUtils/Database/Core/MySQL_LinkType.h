#pragma once

#include "CoreMinimal.h"
#include "MySQL_LinkType.generated.h"


struct MYSQL_TIME;

UENUM(BlueprintType)
enum class EMySQL_TimeFieldType : uint8
{
	Date,			// 仅日期
	Time,			// 仅时间
	DateTime,		// 日期+时间
	TimeStamp,		// MySQL TIMESTAMP
	Year,           // 年份
	NewDate,
	Time2,
	DateTime2,
	TimeStamp2,
	Unknown
};

namespace DedicatedServerUtils
{
	bool IsTypesOfAutomaticRecognition(EMySQL_VariableType InType);

	EMySQL_VariableType FieldTypeToVariableType(const EMySQL_FieldType FieldType);

	bool FStringToMySQLDateTime(const FString& InStr, MYSQL_TIME* InTime, const EMySQL_TimeFieldType InTimeType);
}

UENUM(BlueprintType)
enum class EMySQL_FieldType : uint8
{
	// === 数值类型 ===
	TinyInt				UMETA(DisplayName = "TINYINT (1字节，小整数)"),
	SmallInt			UMETA(DisplayName = "SMALLINT (2字节)"),
	MediumInt			UMETA(DisplayName = "MEDIUMINT (3字节)"),
	Int					UMETA(DisplayName = "INT (4字节)"),
	Integer				UMETA(DisplayName = "INTEGER (4字节，同 INT)"),
	BigInt				UMETA(DisplayName = "BIGINT (8字节，大整数)"),

	Decimal				UMETA(DisplayName = "DECIMAL(M,D) (定点数)"),
	Numeric				UMETA(DisplayName = "NUMERIC(M,D) (定点数，DECIMAL别名)"),
	Float				UMETA(DisplayName = "FLOAT (4字节，单精度浮点数)"),
	Double				UMETA(DisplayName = "DOUBLE (8字节，双精度浮点数)"),
	Real				UMETA(DisplayName = "REAL (通常等同于 DOUBLE)"),

	Bit					UMETA(DisplayName = "BIT(M) (位字段)"),
	Bool				UMETA(DisplayName = "BOOL (布尔，TINYINT(1)别名)"),
	Boolean				UMETA(DisplayName = "BOOLEAN (布尔，TINYINT(1)别名)"),

	// === 日期和时间类型 ===
	Date				UMETA(DisplayName = "DATE (YYYY-MM-DD)"),
	Time				UMETA(DisplayName = "TIME (HH:MM:SS)"),
	Year				UMETA(DisplayName = "YEAR (YYYY)"),
	DateTime			UMETA(DisplayName = "DATETIME (YYYY-MM-DD HH:MM:SS)"),
	TimeStamp			UMETA(DisplayName = "TIMESTAMP (自动更新时间戳)"),

	// === 字符串类型 ===
	Char				UMETA(DisplayName = "CHAR(M) (固定长度字符串)"),
	VarChar				UMETA(DisplayName = "VARCHAR(M) (可变长度字符串)"),

	// === 文本类型 ===
	TinyText			UMETA(DisplayName = "TINYTEXT (最多255字节)"),
	Text				UMETA(DisplayName = "TEXT (最多64KB)"),
	MediumText			UMETA(DisplayName = "MEDIUMTEXT (最多16MB)"),
	LongText			UMETA(DisplayName = "LONGTEXT (最多4GB)"),

	// === 二进制类型 ===
	Binary				UMETA(DisplayName = "BINARY(M) (固定长度二进制串)"),
	VarBinary			UMETA(DisplayName = "VARBINARY(M) (可变长度二进制串)"),

	// === BLOB（二进制大对象）类型 ===
	TinyBlob			UMETA(DisplayName = "TINYBLOB (最多255字节)"),
	Blob				UMETA(DisplayName = "BLOB (最多64KB)"),
	MediumBlob			UMETA(DisplayName = "MEDIUMBLOB (最多16MB)"),
	LongBlob			UMETA(DisplayName = "LONGBLOB (最多4GB)"),

	// === 枚举与集合 ===
	Enum				UMETA(DisplayName = "ENUM('A','B',...) (单选枚举)"),
	Set					UMETA(DisplayName = "SET('A','B',...) (多选集合)"),

	// === JSON 类型 ===
	Json				UMETA(DisplayName = "JSON (JSON文档)"),

	// === 空间（GIS）类型 ===
	Geometry			UMETA(DisplayName = "GEOMETRY (通用几何类型)"),
	Point				UMETA(DisplayName = "POINT (单点，经纬度)"),
	LineString			UMETA(DisplayName = "LINESTRING (线)"),
	Polygon				UMETA(DisplayName = "POLYGON (多边形)"),
	MultiPoint			UMETA(DisplayName = "MULTIPOINT (多点集合)"),
	MultiLineString		UMETA(DisplayName = "MULTILINESTRING (多线集合)"),
	MultiPolygon		UMETA(DisplayName = "MULTIPOLYGON (多多边形集合)"),
	GeometryCollection	UMETA(DisplayName = "GEOMETRYCOLLECTION (几何集合)"),

	// === 其他特殊类型 ===
	Serial				UMETA(DisplayName = "SERIAL (BIGINT UNSIGNED AUTO_INCREMENT 别名)"),
	JsonArray			UMETA(DisplayName = "JSON_ARRAY (JSON数组类型)"),

	// === 未定义或自定义类型 ===
	Unknown				UMETA(DisplayName = "Unknown / 自定义类型")
};

UENUM(BlueprintType)
enum class EMySQL_VariableType : uint8
{
	MYSQL_DECIMAL					UMETA(DisplayName = "Decimal"),		//@@ DECIMAL(M,D)/NUMERIC  M > D ? M+ 2 : D+ 2
	MYSQL_TINY						UMETA(DisplayName = "Small Int"),	//SMALLINT		2 //s
	MYSQL_SHORT						UMETA(DisplayName = "Short"),		//SHORT			2 //s
	MYSQL_LONG						UMETA(DisplayName = "integer"),		//INTEGER		4 //s
	MYSQL_FLOAT						UMETA(DisplayName = "Float"),		//FLOAT			4 //s
	MYSQL_DOUBLE					UMETA(DisplayName = "Double"),		//DOUBLE/REAL	8 //s
	MYSQL_NULL						UMETA(DisplayName = "Null"),		//NULL
	MYSQL_TIMESTAMP					UMETA(DisplayName = "Time Stamp"),	//TIMESTAMP		8
	MYSQL_LONGLONG					UMETA(DisplayName = "Big Int"),		//BIGINT		8 //s
	MYSQL_INT24						UMETA(DisplayName = "Medium Int"),	//MEDIUMINT		3 //ns
	MYSQL_DATE						UMETA(DisplayName = "Date"),		//DATE			3
	MYSQL_TIME						UMETA(DisplayName = "Time"),		//TIME			3
	MYSQL_DATETIME					UMETA(DisplayName = "Date Time"),	//DATETIME		8
	MYSQL_YEAR						UMETA(DisplayName = "Year"),		//YEAR			1
	MYSQL_NEWDATE					UMETA(DisplayName = "New Date"),	//NEWDATE
	MYSQL_VARCHAR					UMETA(DisplayName = "Varchar"),		//VARCHAR
	MYSQL_BIT						UMETA(DisplayName = "Bit"),			//BIT(M)			
	MYSQL_TIMESTAMP2				UMETA(DisplayName = "Time Stamp2"),
	MYSQL_DATETIME2					UMETA(DisplayName = "Date Time2"),
	MYSQL_TIME2						UMETA(DisplayName = "Time2"),
	MYSQL_JSON			= 245		UMETA(DisplayName = "Json"),		//@@
	MYSQL_NEWDECIMAL	= 246		UMETA(DisplayName = "New Decimal"),	//@@
	MYSQL_ENUM			= 247		UMETA(DisplayName = "Enum"),		//				1 or 2   65535个
	MYSQL_SET			= 248		UMETA(DisplayName = "Set"),			//				1,2,4,8
	MYSQL_TINY_BLOB		= 249		UMETA(DisplayName = "Tiny Blob"),	//@@				//s
	MYSQL_MEDIUM_BLOB	= 250		UMETA(DisplayName = "Medium Blob"),	//@@				//s
	MYSQL_LONG_BLOB		= 251		UMETA(DisplayName = "Long Blob"),	//@@				//s
	MYSQL_BLOB			= 252		UMETA(DisplayName = "Blob"),		//@@ 				//s
	MYSQL_VAR_STRING	= 253		UMETA(DisplayName = "VarString"),	//@@ //VARCHAR
	MYSQL_STRING		= 254		UMETA(DisplayName = "String"),		//@@ //CHAR/VARCHAR 
	MYSQL_GEOMETRY		= 255		UMETA(DisplayName = "Geometry"),	//   Geometry(PINT(经度 维度)) 
};

USTRUCT(BlueprintType)
struct DEDICATEDSERVERUTILS_API FMySQL_FieldTypeProperties
{
	GENERATED_BODY()

	FMySQL_FieldTypeProperties() 
		: VariableType(EMySQL_FieldType::VarChar)
		, bIsUnsigned(false)
		, Size(20)
		, DecimalPoint(0)
		, bIsNull(false)
		, bAutoIncrement(false) {}
	FMySQL_FieldTypeProperties(
		const EMySQL_FieldType InVariableType,
		const bool bInIsUnsigned = false,
		const int32 InSize = 0,
		const int32 InDecimalPoint = 0,
		const bool bInIsNull = true,
		const bool bInAutoIncrement = false
	)   : VariableType(InVariableType)
		, bIsUnsigned(bInIsUnsigned)
		, Size(InSize)
		, DecimalPoint(InDecimalPoint)
		, bIsNull(bInIsNull)
		, bAutoIncrement(bInAutoIncrement) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	EMySQL_FieldType VariableType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	bool bIsUnsigned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	int32 Size;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	int32 DecimalPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	bool bIsNull;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString DefaultValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	bool bAutoIncrement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString Comment;

	FString ToString() const;
};

USTRUCT(BlueprintType)
struct DEDICATEDSERVERUTILS_API FMySQL_FieldsData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DedicatedServerUtil|Database")
	FString Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DedicatedServerUtil|Database")
	EMySQL_VariableType Type;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DedicatedServerUtil|Database")
	int32 MaxLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DedicatedServerUtil|Database")
	TArray<FString> DataValues;
};

UENUM(BlueprintType)
enum class EMySQL_SaveEngine : uint8
{
	//___________________________________________________________________________________________________________________________________
	//Engine			//|Comment														 |Support	| Transactions  |XA		|Savepoints	|			
	//____________________|_______________________________________________________________________________________________________________
	InnoDB,				//|Supports transactions, row-level locking, and foreign keys	 |	YES		|	YES			|YES	|	YES		|
	MyISAM,				//|MyISAM storage engine										 |	DEFAULT	|	NO			|NO		|	NO		|
	MEMORY,				//|Hash based, stored in memory, useful for temporary tables	 |	YES		|	NO			|NO		|	NO		|
	ARCHIVE,			//|Archive storage engine										 |	YES		|	NO			|NO		|	NO		|
	CSV,				//|CSV storage engine											 |	YES		|	NO			|NO		|	NO		|
	PERFORMANCE_SCHEMA,	//|Performance Schema											 |	YES		|	NO			|NO		|	NO		|
	MRG_MYISAM,			//|Collection of identical MyISAM tables						 |	YES		|	NO			|NO		|	NO		|
	BLACKHOLE,			//|/dev/null storage engine (anything you write to it disappears)|	YES		|	NO			|NO		|	NO		|
	FEDERATED,			//|Federated MySQL storage engine								 |	NO		|	NULL		|NULL	|	NULL	|
	//___________________________________________________________________________________________________________________________________
};

UENUM(BlueprintType)
enum class EMySQL_Charset : uint8
{
	None,
	armscii8,
	ascii,
	big5,
	binary,
	cp1250,
	cp1251,
	cp1256,
	cp1257,
	cp850,
	cp852,
	cp866,
	cp932,
	dec8,
	eucjpms,
	euckr,
	gb2312,
	gbk,
	geostd8,
	greek,
	bebrew,
	hp8,
	keybcs2,
	koi8r,
	koi8u,
	latin1,
	latin2,
	latin5,
	latin7,
	macce,
	macroman,
	sjis,
	swe7,
	tis620,
	ucs2,
	ujis,
	utf16,
	utf32,
	utf8,
	utf8mb4
};

USTRUCT(BlueprintType)
struct DEDICATEDSERVERUTILS_API FMySQL_TableOptions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	EMySQL_SaveEngine Engine = EMySQL_SaveEngine::InnoDB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	EMySQL_Charset Charset = EMySQL_Charset::utf8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	int64 AutoIncrementStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString Comment;

	FString ToString() const;
};

/*
* 保证此处枚举值与官方枚举值顺序一致
*/
UENUM(BlueprintType)
enum class EMySQL_ClientFlag : uint8
{
	Client_Long_Password,		/* 使用更安全的加密密码 */
	Client_Found_Rows,			/* 返回“找到的行数”而不是“受影响的行数” */
	Client_Long_Flag,			/* 获取所有列的详细标志信息 */
	Client_Connect_With_DB,		/* 在连接时可以直接指定数据库 */
	Client_No_Schema,			/* 不允许使用 database.table.column 这种带库前缀的列名格式 */
	Client_Compress,			/* 启用压缩协议 */
	Client_ODBC,				/* ODBC 客户端 */
	Client_Local_Files,			/* 允许使用 LOAD DATA LOCAL INFILE 从本地文件导入数据 */
	Client_Ignore_Space,		/* 在函数名后允许空格（例如允许 "COUNT (*)"） */
	Client_Protocol_41,			/* New 4.1 protocol */
	Client_Interactive,			/* 标记为交互式客户端（服务器会设置更长的超时时间） */
	Client_SSL,					/* 握手完成后切换到 SSL 加密通信 */
	Client_Ignore_Sigpipe,		/* 忽略 SIGPIPE 信号（防止写入关闭的 socket 时程序被终止） */
	Client_Transactions,		/* 客户端支持事务（InnoDB 或其他事务型引擎） */
	Client_Reserved,			/* 保留字段（旧协议遗留标志，用于 4.1 协议）  */
	Client_Reserved2,			/* 保留字段（旧版本认证兼容用） */
	Client_Multi_Statements,	/* 启用多语句支持 */
	Client_Multi_Results,		/* 启用多结果集 */
	Client_PS_Multi_Results,	/* M启用预处理语句（Prepared Statement）下的多结果集返回 */
	Client_Plugin_Auth,			/* 支持插件式认证 */
	client_connect_attrs,		/* 支持连接属性 */
	Client_Plugin_Auth_Lenenc_Client_Data,	/* 支持更长的认证数据包 */
	Client_Can_Handle_Expired_Passwords,	/* 支持处理过期密码 */
	Client_Session_Track,		/* 支持会话状态跟踪 */
	Client_Deprecate_EOF,		/* 不再使用 EOF 包 */
	Client_Optional_Resultset_Metadata,		/* 允许服务器省略部分结果集元数据*/
	Client_Ssl_Verify_Server_Cert = 30,		/* 启用服务器证书验证 */
	Client_Remember_Options = 31			/* 在连接失败后保留客户端选项 */
};

UENUM(BlueprintType)
enum class EMySQL_SelectJoinMode : uint8
{
	Inner,
	Left,
	Right,
	Cross
};

USTRUCT(BlueprintType)
struct DEDICATEDSERVERUTILS_API FMySQL_JoinParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	EMySQL_SelectJoinMode JoinMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString TableName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString Conditions;

	FString ToString() const;
};

USTRUCT(BlueprintType)
struct DEDICATEDSERVERUTILS_API FMySQL_LimitParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	int32 LimitOffset = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	int32 LimitNum = -1;

	FString ToString() const;
};

USTRUCT(BlueprintType)
struct DEDICATEDSERVERUTILS_API FMySQL_UpdateParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	TMap<FString, FString> DataToUpdate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	TArray<FMySQL_JoinParams> Joins;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString Conditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	TMap<FString, bool> OrdersAndIsDesc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FMySQL_LimitParams Limit;

	FString ToString() const;
};

USTRUCT(BlueprintType)
struct DEDICATEDSERVERUTILS_API FMySQL_UpdateReplaceData
{
	GENERATED_BODY()

	FMySQL_UpdateReplaceData() {}
	FMySQL_UpdateReplaceData(const FString& InField, const FString& InOld, const FString& InNew)
		: FieldName(InField), OldSubString(InOld), NewSubString(InNew)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString FieldName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString OldSubString;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString NewSubString;

	FString ToString() const;
};

USTRUCT(BlueprintType)
struct DEDICATEDSERVERUTILS_API FMySQL_UpdateReplaceParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	TArray<FMySQL_UpdateReplaceData> DataToReplace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	TArray<FMySQL_JoinParams> Joins;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString Conditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	TMap<FString, bool> OrdersAndIsDesc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FMySQL_LimitParams Limit;

	FString ToString() const;
};

UENUM(BlueprintType)
enum class EMySQL_AlterColumnOpt : uint8
{
	ADD,
	DROP,
	MODIFY,
	CHANGE
};

USTRUCT(BlueprintType)
struct DEDICATEDSERVERUTILS_API FMySQL_AlterColumnParams
{
	GENERATED_BODY()

	FMySQL_AlterColumnParams() {}
	FMySQL_AlterColumnParams(
		const EMySQL_AlterColumnOpt InAlterOpt,
		const FString& InFieldName,
		const FMySQL_FieldTypeProperties& InFieldType = FMySQL_FieldTypeProperties(),
		const FString& InNewFieldName = TEXT(""),
		const FString& InExtra = TEXT("")
	)   : AlterOpt(InAlterOpt)
		, FieldName(InFieldName)
		, FieldType(InFieldType)
		, NewFieldName(InNewFieldName)
		, Extra(InExtra) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	EMySQL_AlterColumnOpt AlterOpt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString FieldName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FMySQL_FieldTypeProperties FieldType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString NewFieldName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString Extra;

	FString ToString() const;
};

UENUM(BlueprintType)
enum class EMySQL_IndexType : uint8
{
	INDEX,
	UNIQUE,
	FULLTEXT,
	SPATIAL
};

USTRUCT(BlueprintType)
struct DEDICATEDSERVERUTILS_API FMySQL_AlterIndexParams
{
	GENERATED_BODY()

	FMySQL_AlterIndexParams() {}
	FMySQL_AlterIndexParams(
		const bool bInIsAdd, 
		const bool bInIsPrimary = false,
		const EMySQL_IndexType InIndexType = EMySQL_IndexType::INDEX,
		const FString& InIndexName = TEXT(""),
		const TArray<FString>& InColumns = {}
	)   : bIsAdd(bInIsAdd)
		, bIsPrimaryKey(bInIsPrimary)
		, IndexType(InIndexType)
		, IndexName(InIndexName)
		, Columns(InColumns) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	bool bIsAdd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	bool bIsPrimaryKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	EMySQL_IndexType IndexType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString IndexName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	TArray<FString> Columns;

	FString ToString() const;
};

USTRUCT(BlueprintType)
struct DEDICATEDSERVERUTILS_API FMySQL_AlterForeingParams
{
	GENERATED_BODY()

	FMySQL_AlterForeingParams()
		: bIsAdd(false)
		, OnDelete(TEXT("CASCADE"))
		, OnUpdate(TEXT("CASCADE")) {}
	FMySQL_AlterForeingParams(
		const bool bInIsAdd,
		const FString& InForeingKey,
		const TArray<FString>& InChildFields,
		const FString& InParentTable,
		const TArray<FString>& InParentFields,
		const FString& InOnDelete = TEXT("CASCADE"),
		const FString& InOnUpdate = TEXT("CASCADE")
	)   : bIsAdd(bInIsAdd)
		, ForeingKey(InForeingKey)
		, ChildFields(InChildFields)
		, ParentTable(InParentTable)
		, ParentFields(InParentFields)
		, OnDelete(InOnDelete)
		, OnUpdate(InOnUpdate) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	bool bIsAdd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString ForeingKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	TArray<FString> ChildFields;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString ParentTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	TArray<FString> ParentFields;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString OnDelete;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString OnUpdate;

	FString ToString() const;
};

USTRUCT(BlueprintType)
struct DEDICATEDSERVERUTILS_API FMySQL_StmtValue
{
	GENERATED_BODY()

	FMySQL_StmtValue() {}
	FMySQL_StmtValue(const FString& InValue, const EMySQL_FieldType InFieldType)
		: Value(InValue), FieldType(InFieldType) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	FString Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DedicatedServerUtil|Database")
	EMySQL_FieldType FieldType;

	uint32 GetValueTypeLen() const;
	unsigned long* GetValueTypeLenMemory() const;
	void GetValue(void** InValue) const;
};