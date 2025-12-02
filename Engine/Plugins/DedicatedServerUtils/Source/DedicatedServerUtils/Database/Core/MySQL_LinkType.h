#pragma once

#include "CoreMinimal.h"
#include "MySQL_LinkType.generated.h"


UENUM(BlueprintType)
enum class EMySQL_VariableType : uint8
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

//UENUM(BlueprintType)
//enum class EMySQL_VariableType :uint8
//{
//	MYSQL_DECIMAL					UMETA(DisplayName = "Decimal"),		//DECIMAL(M,D)/NUMERIC  M > D ? M+ 2 : D+ 2
//	MYSQL_TINY						UMETA(DisplayName = "Small Int"),	//SMALLINT		2 //s
//	MYSQL_SHORT						UMETA(DisplayName = "Short"),		//SHORT			2 //s
//	MYSQL_LONG						UMETA(DisplayName = "integer"),		//INTEGER		4 //s
//	MYSQL_FLOAT						UMETA(DisplayName = "Float"),		//FLOAT			4 //s
//	MYSQL_DOUBLE					UMETA(DisplayName = "Double"),		//DOUBLE/REAL	8 //s
//	MYSQL_NULL						UMETA(DisplayName = "Null"),		//NULL
//	MYSQL_TIMESTAMP					UMETA(DisplayName = "Time Stamp"),	//TIMESTAMP		8
//	MYSQL_LONGLONG					UMETA(DisplayName = "Big Int"),		//BIGINT		8 //s
//	MYSQL_INT24						UMETA(DisplayName = "Medium Int"),	//MEDIUMINT		3 //ns
//	MYSQL_DATE						UMETA(DisplayName = "Date"),		//DATE			3
//	MYSQL_TIME						UMETA(DisplayName = "Time"),		//TIME			3
//	MYSQL_DATETIME					UMETA(DisplayName = "Date Time"),	//DATETIME		8
//	MYSQL_YEAR						UMETA(DisplayName = "Year"),		//YEAR			1
//	MYSQL_NEWDATE					UMETA(DisplayName = "New Date"),	//NEWDATE
//	MYSQL_VARCHAR					UMETA(DisplayName = "Varchar"),		//VARCHAR
//	MYSQL_BIT						UMETA(DisplayName = "Bit"),			//BIT(M)			
//	MYSQL_TIMESTAMP2				UMETA(DisplayName = "Time Stamp2"),
//	MYSQL_DATETIME2					UMETA(DisplayName = "Date Time2"),
//	MYSQL_TIME2						UMETA(DisplayName = "Time2"),
//	MYSQL_JSON			= 245		UMETA(DisplayName = "Json"),		//@@
//	MYSQL_NEWDECIMAL	= 246		UMETA(DisplayName = "New Decimal"),	//
//	MYSQL_ENUM			= 247		UMETA(DisplayName = "Enum"),		//				1 or 2   65535个
//	MYSQL_SET			= 248		UMETA(DisplayName = "Set"),			//				1,2,4,8
//	MYSQL_TINY_BLOB		= 249		UMETA(DisplayName = "Tiny Blob"),	//@@				//s
//	MYSQL_MEDIUM_BLOB	= 250		UMETA(DisplayName = "Medium Blob"),	//@@				//s
//	MYSQL_LONG_BLOB		= 251		UMETA(DisplayName = "Long Blob"),	//@@				//s
//	MYSQL_BLOB			= 252		UMETA(DisplayName = "Blob"),		//@@ 				//s
//	MYSQL_VAR_STRING	= 253		UMETA(DisplayName = "VarString"),	//@@ //VARCHAR
//	MYSQL_STRING		= 254		UMETA(DisplayName = "String"),		//@@ //CHAR/VARCHAR 
//	MYSQL_GEOMETRY		= 255		UMETA(DisplayName = "Geometry"),	//   Geometry(PINT(经度 维度)) 
//};

USTRUCT(BlueprintType)
struct FMySQL_FieldType
{
	GENERATED_BODY()

	FMySQL_FieldType();

	UPROPERTY()
	EMySQL_VariableType VariableType;

	UPROPERTY()
	bool bIsUnsigned;

	UPROPERTY()
	int32 Size;

	UPROPERTY()
	int32 DecimalPoint;

	UPROPERTY()
	bool bIsNull;

	UPROPERTY()
	FString DefaultValue;

	UPROPERTY()
	bool bAutoIncrement;

	UPROPERTY()
	FString Comment;

	FString ToString() const;
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
struct FMySQL_TableOptions
{
	GENERATED_BODY()

	UPROPERTY()
	EMySQL_SaveEngine Engine = EMySQL_SaveEngine::InnoDB;

	UPROPERTY()
	EMySQL_Charset Charset = EMySQL_Charset::utf8;

	UPROPERTY()
	int64 AutoIncrementStart;

	UPROPERTY()
	FString Comment;

	FString ToString() const;
};