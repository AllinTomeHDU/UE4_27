#include "MySQL_LinkType.h"
#include "UObject/EnumProperty.h"

#ifdef TEXT
#undef TEXT  
#include "Windows/AllowWindowsPlatformAtomics.h"
#include "../Official/mysql.h"
#include "Windows/HideWindowsPlatformAtomics.h"
#endif

using namespace DedicatedServerUtils;


#if PLATFORM_WINDOWS
#pragma optimize("",off)
#endif
bool DedicatedServerUtils::IsTypesOfAutomaticRecognition(EMySQL_VariableType InType)
{
	uint8  Type =  (uint8)InType;
	return Type != (uint8)EMySQL_VariableType::MYSQL_DECIMAL
		&& Type != (uint8)EMySQL_VariableType::MYSQL_JSON
		&& Type != (uint8)EMySQL_VariableType::MYSQL_NEWDECIMAL
		&& !(Type >=  (uint8)EMySQL_VariableType::MYSQL_TINY_BLOB && Type < (uint8)EMySQL_VariableType::MYSQL_GEOMETRY);
}

EMySQL_VariableType DedicatedServerUtils::FieldTypeToVariableType(const EMySQL_FieldType FieldType)
{
	switch (FieldType)
	{
	case EMySQL_FieldType::TinyInt:        return EMySQL_VariableType::MYSQL_TINY;
	case EMySQL_FieldType::SmallInt:       return EMySQL_VariableType::MYSQL_SHORT;
	case EMySQL_FieldType::MediumInt:      return EMySQL_VariableType::MYSQL_INT24;
	case EMySQL_FieldType::Int:
	case EMySQL_FieldType::Integer:        return EMySQL_VariableType::MYSQL_LONG;
	case EMySQL_FieldType::BigInt:         return EMySQL_VariableType::MYSQL_LONGLONG;

	case EMySQL_FieldType::Decimal:        return EMySQL_VariableType::MYSQL_NEWDECIMAL;
	case EMySQL_FieldType::Numeric:        return EMySQL_VariableType::MYSQL_NEWDECIMAL;
	case EMySQL_FieldType::Float:          return EMySQL_VariableType::MYSQL_FLOAT;
	case EMySQL_FieldType::Double:
	case EMySQL_FieldType::Real:           return EMySQL_VariableType::MYSQL_DOUBLE;

	case EMySQL_FieldType::Bit:            return EMySQL_VariableType::MYSQL_BIT;
	case EMySQL_FieldType::Bool:
	case EMySQL_FieldType::Boolean:        return EMySQL_VariableType::MYSQL_TINY;

	case EMySQL_FieldType::Date:           return EMySQL_VariableType::MYSQL_DATE;
	case EMySQL_FieldType::Time:           return EMySQL_VariableType::MYSQL_TIME;
	case EMySQL_FieldType::DateTime:       return EMySQL_VariableType::MYSQL_DATETIME;
	case EMySQL_FieldType::TimeStamp:      return EMySQL_VariableType::MYSQL_TIMESTAMP;
	case EMySQL_FieldType::Year:           return EMySQL_VariableType::MYSQL_YEAR;

	case EMySQL_FieldType::Char:           return EMySQL_VariableType::MYSQL_STRING;
	case EMySQL_FieldType::VarChar:        return EMySQL_VariableType::MYSQL_VAR_STRING;

	case EMySQL_FieldType::TinyText:       return EMySQL_VariableType::MYSQL_TINY_BLOB;
	case EMySQL_FieldType::Text:           return EMySQL_VariableType::MYSQL_BLOB;
	case EMySQL_FieldType::MediumText:     return EMySQL_VariableType::MYSQL_MEDIUM_BLOB;
	case EMySQL_FieldType::LongText:       return EMySQL_VariableType::MYSQL_LONG_BLOB;

	case EMySQL_FieldType::Binary:         return EMySQL_VariableType::MYSQL_BLOB;
	case EMySQL_FieldType::VarBinary:      return EMySQL_VariableType::MYSQL_VAR_STRING;

	case EMySQL_FieldType::TinyBlob:       return EMySQL_VariableType::MYSQL_TINY_BLOB;
	case EMySQL_FieldType::Blob:           return EMySQL_VariableType::MYSQL_BLOB;
	case EMySQL_FieldType::MediumBlob:     return EMySQL_VariableType::MYSQL_MEDIUM_BLOB;
	case EMySQL_FieldType::LongBlob:       return EMySQL_VariableType::MYSQL_LONG_BLOB;

	case EMySQL_FieldType::Enum:           return EMySQL_VariableType::MYSQL_ENUM;
	case EMySQL_FieldType::Set:            return EMySQL_VariableType::MYSQL_SET;

	case EMySQL_FieldType::Json:
	case EMySQL_FieldType::JsonArray:      return EMySQL_VariableType::MYSQL_JSON;

	case EMySQL_FieldType::Geometry:
	case EMySQL_FieldType::Point:
	case EMySQL_FieldType::LineString:
	case EMySQL_FieldType::Polygon:
	case EMySQL_FieldType::MultiPoint:
	case EMySQL_FieldType::MultiLineString:
	case EMySQL_FieldType::MultiPolygon:
	case EMySQL_FieldType::GeometryCollection:
		return EMySQL_VariableType::MYSQL_GEOMETRY;

	case EMySQL_FieldType::Serial:         return EMySQL_VariableType::MYSQL_LONGLONG;

	default:                               return EMySQL_VariableType::MYSQL_NULL;
	}
	return EMySQL_VariableType::MYSQL_NULL;
}

bool DedicatedServerUtils::FStringToMySQLDateTime(const FString& InStr, MYSQL_TIME* InTime, const EMySQL_TimeFieldType InTimeType)
{
	if (!InTime) return false;
	
	FString DateTimeStr = InStr;
	DateTimeStr.ReplaceInline(TEXT("-"), TEXT(" "));
	DateTimeStr.ReplaceInline(TEXT(":"), TEXT(" "));
	DateTimeStr.ReplaceInline(TEXT("."), TEXT(" "));

	TArray<FString> Tokens;
	DateTimeStr.ParseIntoArray(Tokens, TEXT(" "), true);

	switch (InTimeType)
	{
	case EMySQL_TimeFieldType::Date:
		{
			if (Tokens.Num() != 3) return false;
			InTime->year = FCString::Atoi(*Tokens[0]);
			InTime->month = FCString::Atoi(*Tokens[1]);
			InTime->day = FCString::Atoi(*Tokens[2]);
		}
		break;
	case EMySQL_TimeFieldType::Time:
		{
			if (Tokens.Num() != 3) return false;
			InTime->hour = FCString::Atoi(*Tokens[0]);
			InTime->minute = FCString::Atoi(*Tokens[1]);
			InTime->second = FCString::Atoi(*Tokens[2]);
		}
		break;
	case EMySQL_TimeFieldType::DateTime:
		{
			if (Tokens.Num() != 6) return false;
			InTime->year = FCString::Atoi(*Tokens[0]);
			InTime->month = FCString::Atoi(*Tokens[1]);
			InTime->day = FCString::Atoi(*Tokens[2]);
			InTime->hour = FCString::Atoi(*Tokens[3]);
			InTime->minute = FCString::Atoi(*Tokens[4]);
			InTime->second = FCString::Atoi(*Tokens[5]);
		}
		break;
	default:
		return false;
	}
	return true;
}

FString FMySQL_FieldTypeProperties::ToString() const
{
	/*
	* 基本的类型相关规定
	*/
	{
		// 1. 部分类型必须指定长度
		// 2. auto_increment为true不能再设置默认值，可在表选项或通过ALERT设置递增起始值
	}

	FString FieldTypeString = StaticEnum<EMySQL_FieldType>()->GetNameStringByIndex((int32)VariableType);

	if (Size > 0 || DecimalPoint > 0)
	{
		FieldTypeString += TEXT("(") + FString::Printf(TEXT("%d"), Size) + 
			(DecimalPoint > 0 ? FString::Printf(TEXT(",%d"),DecimalPoint) : FString()) + TEXT(")");
	}

	if (bIsUnsigned)
	{
		FieldTypeString += TEXT(" UNSIGNED");
	}

	if (bIsNull)
	{
		FieldTypeString += TEXT(" NULL");
	}
	else
	{
		FieldTypeString += TEXT(" NOT NULL");
		if (!DefaultValue.IsEmpty())
		{
			FieldTypeString += TEXT(" DEFAULT ") + DefaultValue;
		}
		if (bAutoIncrement)
		{
			FieldTypeString += TEXT(" AUTO_INCREMENT");
		}
	}

	if (!Comment.IsEmpty())
	{
		FieldTypeString += TEXT(" COMMENT '") + Comment + TEXT("'");
	}

	return FieldTypeString;
}

FString FMySQL_TableOptions::ToString() const
{
	FString SQL = TEXT(" ENGINE=") + StaticEnum<EMySQL_SaveEngine>()->GetNameStringByIndex((int32)Engine);
	if (AutoIncrementStart != 0)
	{
		SQL += TEXT(" AUTO_INCREMENT=") + FString::Printf(TEXT("%lld"), AutoIncrementStart);
	}
	if (Charset != EMySQL_Charset::None)
	{
		SQL += TEXT(" DEFAULT CHARSET=") + StaticEnum<EMySQL_Charset>()->GetNameStringByIndex((int32)Charset);
	}
	if (!Comment.IsEmpty())
	{
		SQL += TEXT(" COMMENT '") + Comment + TEXT("'");
	}
	return SQL;
}

FString FMySQL_JoinParams::ToString() const
{
	FString SQL = TEXT(" ");
	SQL += StaticEnum<EMySQL_SelectJoinMode>()->GetNameStringByIndex((int32)JoinMode) + TEXT(" JOIN ");
	SQL += TEXT("`") + TableName + TEXT("`");
	if (JoinMode != EMySQL_SelectJoinMode::Cross && !Conditions.IsEmpty())
	{
		SQL += TEXT(" ON ") + Conditions;
	}
	return SQL;
}

FString FMySQL_LimitParams::ToString() const
{
	FString SQL = TEXT(" LIMIT ");
	SQL += LimitOffset > 0 ? FString::Printf(TEXT("%i,%i"), LimitOffset, LimitNum)
						   : FString::Printf(TEXT("%i"), LimitNum);
	return SQL;
}

FString FMySQL_UpdateParams::ToString() const
{
	FString SQL;
	for (auto& Tmp : Joins)
	{
		SQL += Tmp.ToString();
	}
	SQL += TEXT(" SET ");
	for (auto& Tmp : DataToUpdate)
	{
		SQL += TEXT("`") + Tmp.Key + TEXT("`=") + Tmp.Value + TEXT(",");
	}
	SQL.RemoveFromEnd(TEXT(","));
	if (!Conditions.IsEmpty())
	{
		SQL += TEXT(" WHERE ") + Conditions;
	}
	if (OrdersAndIsDesc.Num() > 0)
	{
		SQL += TEXT(" ORDER BY ");
		for (auto& Tmp : OrdersAndIsDesc)
		{
			SQL += TEXT("`") + Tmp.Key + (Tmp.Value ? TEXT("` DESC,") : TEXT("`,"));
		}
		SQL.RemoveFromEnd(",");
	}
	if (Limit.LimitNum >= 0)
	{
		SQL += Limit.ToString();
	}
	return SQL;
}

FString FMySQL_UpdateReplaceData::ToString() const
{
	FString SQL = TEXT(" `") + FieldName + TEXT("`=REPLACE(`") + FieldName + TEXT("`,");
	SQL += TEXT("'") + OldSubString + TEXT("',");
	SQL += TEXT("'") + NewSubString + TEXT("')");
	return SQL;
}

FString FMySQL_UpdateReplaceParams::ToString() const
{
	FString SQL;
	for (auto& Tmp : Joins)
	{
		SQL += Tmp.ToString();
	}
	SQL += TEXT(" SET");
	for (auto& Tmp : DataToReplace)
	{
		SQL += Tmp.ToString() + TEXT(",");
	}
	SQL.RemoveFromEnd(TEXT(","));
	if (!Conditions.IsEmpty())
	{
		SQL += TEXT(" WHERE ") + Conditions;
	}
	if (OrdersAndIsDesc.Num() > 0)
	{
		SQL += TEXT(" ORDER BY ");
		for (auto& Tmp : OrdersAndIsDesc)
		{
			SQL += TEXT("`") + Tmp.Key + (Tmp.Value ? TEXT("` DESC,") : TEXT("`,"));
		}
		SQL.RemoveFromEnd(",");
	}
	if (Limit.LimitNum >= 0)
	{
		SQL += Limit.ToString();
	}
	return SQL;
}

FString FMySQL_AlterColumnParams::ToString() const
{
	FString SQL = StaticEnum<EMySQL_AlterColumnOpt>()->GetNameStringByIndex((int32)AlterOpt);
	SQL += TEXT(" `") + FieldName + TEXT("`");
	if (AlterOpt != EMySQL_AlterColumnOpt::DROP && AlterOpt != EMySQL_AlterColumnOpt::CHANGE)
	{
		SQL += TEXT(" ") + FieldType.ToString();
	}
	else if (AlterOpt == EMySQL_AlterColumnOpt::CHANGE)
	{
		SQL += TEXT(" `") + NewFieldName + TEXT("` ") + FieldType.ToString();
	}
	SQL += TEXT(" ") + Extra;
	return SQL;
}

FString FMySQL_AlterIndexParams::ToString() const
{
	FString SQL = bIsAdd ? TEXT("ADD ") : TEXT("DROP ");
	if (bIsPrimaryKey)
	{
		SQL += TEXT("PRIMARY KEY ");
	}
	else
	{
		SQL += StaticEnum<EMySQL_IndexType>()->GetNameStringByIndex((int32)IndexType);
		if (IndexType != EMySQL_IndexType::INDEX)
			SQL += TEXT(" INDEX");
		SQL += TEXT(" ") + IndexName;
	}
	if (bIsAdd)
	{
		SQL += TEXT(" (");
		for (auto& Tmp : Columns)
		{
			SQL += TEXT("`") + Tmp + TEXT("`,");
		}
		SQL.RemoveFromEnd(TEXT(","));
		SQL += TEXT(")");
	}
	return SQL;
}

FString FMySQL_AlterForeingParams::ToString() const
{
	FString SQL = bIsAdd ? TEXT("ADD CONSTRAINT `") : TEXT("DROP FOREING KEY `");
	SQL += ForeingKey + TEXT("`");
	if (bIsAdd)
	{
		SQL += TEXT(" FOREIGN KEY (");
		for (auto& Tmp : ChildFields)
		{
			SQL += TEXT("`") + Tmp + TEXT("`,");
		}
		SQL.RemoveFromEnd(TEXT(","));
		SQL += TEXT(") REFERENCES `") + ParentTable + TEXT("`(");
		for (auto& Tmp : ParentFields)
		{
			SQL += TEXT("`") + Tmp + TEXT("`,");
		}
		SQL.RemoveFromEnd(TEXT(","));
		SQL += TEXT(" ON DELETE ") + OnDelete;
		SQL += TEXT(" ON UPDATE ") + OnUpdate;
	}
	SQL += TEXT(";");
	return SQL;
}

uint32 FMySQL_StmtValue::GetValueTypeLen() const
{
	if (!DedicatedServerUtils::IsTypesOfAutomaticRecognition(FieldTypeToVariableType(FieldType)))
	{
		return Value.Len();
	}
	return 0;
}

unsigned long* FMySQL_StmtValue::GetValueTypeLenMemory() const
{
	if (!DedicatedServerUtils::IsTypesOfAutomaticRecognition(FieldTypeToVariableType(FieldType)))
	{
		return nullptr;
	}
	unsigned long* Size = (unsigned long*)FMemory::Malloc(sizeof(unsigned long));
	*Size = GetValueTypeLen();
	return Size;
}

void FMySQL_StmtValue::GetValue(void** InValue) const
{
	EMySQL_VariableType VariableType = FieldTypeToVariableType(FieldType);
	switch (VariableType)
	{
	case EMySQL_VariableType::MYSQL_DECIMAL:
		return;
	case EMySQL_VariableType::MYSQL_TINY:
		{
			char* Data = (char*)FMemory::Malloc(sizeof(char));
			*Data = FCString::Atoi(*Value);
			*InValue = Data;
		}
		return;
	case EMySQL_VariableType::MYSQL_SHORT:
		{
			short* Data = (short*)FMemory::Malloc(sizeof(short));
			*Data = (short)FCString::Atoi(*Value);
			*InValue = Data;
		}
		return;
	//	case EMySQL_VariableType::MYSQL_INT24:
	case EMySQL_VariableType::MYSQL_LONG:
		{
			int32* Data = (int32*)FMemory::Malloc(sizeof(int32));
			*Data = FCString::Atoi(*Value);
			*InValue = Data;
		}
		return;
	case EMySQL_VariableType::MYSQL_FLOAT:
		{
			float* Data = (float*)FMemory::Malloc(sizeof(float));
			*Data = FCString::Atof(*Value);
			*InValue = Data;
		}
		return;
	case EMySQL_VariableType::MYSQL_DOUBLE:
		{
			double* Data = (double*)FMemory::Malloc(sizeof(double));
			*Data = FCString::Atod(*Value);
			*InValue = Data;
		}
		return;
	case EMySQL_VariableType::MYSQL_LONGLONG:
		{
			long long* Data = (long long*)FMemory::Malloc(sizeof(long long));
			*Data = FCString::Atoi64(*Value);
			*InValue = Data;
		}
		return;
	case EMySQL_VariableType::MYSQL_TIMESTAMP:
		return;
	case EMySQL_VariableType::MYSQL_DATE:
		{
			MYSQL_TIME* TimeData = (MYSQL_TIME*)FMemory::Malloc(sizeof(MYSQL_TIME));
			TimeData->time_type = MYSQL_TIMESTAMP_DATE;
			FStringToMySQLDateTime(Value, TimeData, EMySQL_TimeFieldType::Date);
			*InValue = TimeData;
		}
		return;
	case EMySQL_VariableType::MYSQL_TIME:
		{
			MYSQL_TIME* TimeData = (MYSQL_TIME*)FMemory::Malloc(sizeof(MYSQL_TIME));
			TimeData->time_type = MYSQL_TIMESTAMP_TIME;
			FStringToMySQLDateTime(Value, TimeData, EMySQL_TimeFieldType::Time);
			*InValue = TimeData;
		}
		return;
	case EMySQL_VariableType::MYSQL_DATETIME:
		{
			MYSQL_TIME* TimeData = (MYSQL_TIME*)FMemory::Malloc(sizeof(MYSQL_TIME));
			TimeData->time_type = MYSQL_TIMESTAMP_DATETIME;
			FStringToMySQLDateTime(Value, TimeData, EMySQL_TimeFieldType::DateTime);
			*InValue = TimeData;
		}
		return;
	case EMySQL_VariableType::MYSQL_YEAR:
		return;
	case EMySQL_VariableType::MYSQL_NEWDATE:
		return;
	case EMySQL_VariableType::MYSQL_BIT:
		return;
	case EMySQL_VariableType::MYSQL_TIMESTAMP2:
		return;
	case EMySQL_VariableType::MYSQL_DATETIME2:
		return;
	case EMySQL_VariableType::MYSQL_TIME2:
		return;
	case EMySQL_VariableType::MYSQL_NEWDECIMAL:
		return;
	case EMySQL_VariableType::MYSQL_ENUM:
		return;
	case EMySQL_VariableType::MYSQL_SET:
		return;
	case EMySQL_VariableType::MYSQL_VARCHAR:
	case EMySQL_VariableType::MYSQL_JSON:
	case EMySQL_VariableType::MYSQL_TINY_BLOB:
	case EMySQL_VariableType::MYSQL_MEDIUM_BLOB:
	case EMySQL_VariableType::MYSQL_LONG_BLOB:
	case EMySQL_VariableType::MYSQL_BLOB:
	case EMySQL_VariableType::MYSQL_VAR_STRING:
	case EMySQL_VariableType::MYSQL_STRING:
		{
			int32 Len = Value.Len();
			char* CharValue = TCHAR_TO_ANSI(const_cast<TCHAR*>(Value.GetCharArray().GetData()));
			char* Data = (char*)FMemory::Malloc(Len);
			FMemory::Memzero(Data, Len);
			FMemory::Memcpy(Data, CharValue, Len + 1);
			*InValue = Data;
		}
		return;
	case EMySQL_VariableType::MYSQL_GEOMETRY:
		return;
	}

	*InValue = nullptr;
}
#if PLATFORM_WINDOWS
#pragma optimize("",on)
#endif