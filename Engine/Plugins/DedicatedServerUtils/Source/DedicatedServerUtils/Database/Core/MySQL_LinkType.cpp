#include  "MySQL_LinkType.h"
#include "UObject/EnumProperty.h"


FMySQL_FieldType::FMySQL_FieldType()
	: VariableType(EMySQL_VariableType::VarChar)
	, bIsUnsigned(false)
	, Size(0)
	, DecimalPoint(0)
	, bIsNull(true)
	, bAutoIncrement(false)
{
}

FString FMySQL_FieldType::ToString() const
{
	/*
	* 基本的类型相关规定
	*/
	{
		// 1. 部分类型必须指定长度
		// 2. auto_increment为true不能再设置默认值，可在表选项或通过ALERT设置递增起始值
	}

	FString FieldTypeString = StaticEnum<EMySQL_VariableType>()->GetNameStringByIndex((int32)VariableType);

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
