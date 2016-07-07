// CAdoOperate.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "CAdo.h"
#include "LogFile.h"

CAdo::CAdo()
{
	isConn=FALSE;
	//初始化COM组件
    hr=::CoInitialize(NULL);
}
CAdo::CAdo(char *connStr,char *userName,char *passWord)
{
     hr=::CoInitialize(NULL);
	 isConn=FALSE;
	 if(!Connect(connStr,userName,passWord))
	 {
         return;
	 }
}
CAdo::~CAdo()
{
	Close();
	::CoUninitialize();
}
BOOL CAdo::Connect(char *connStr,char *userName,char *passWord)
{
	//如果isConn是TRUE，表示已经连接过了
	if(isConn)
		return TRUE;
	//如果连接字符串为空，则直接返回失败
	if(connStr==NULL)
		return FALSE;
	try
	{
		//创建连接
		conn.CreateInstance(__uuidof(Connection));
		conn->Open(connStr,userName,passWord,-1);
		//创建命令
		command.CreateInstance(__uuidof(Command));
		command->ActiveConnection=conn;
		//创建结果集
		 rs.CreateInstance(__uuidof(Recordset));
		 //表示建立了连接
		 isConn=TRUE;
		 return TRUE;
	}
	catch(_com_error &e)
	{
		LOG("[Db][%s]", (char*)e.Description());
        if(conn->State)
		{
			conn->Close();
			conn=NULL;
		}
		return FALSE;
    }
}
BOOL CAdo::Close()
{
	//如果已经关掉了
	if(!isConn)
		return TRUE;
	//否则，关闭数据库连接
	try
	{
		conn->Close();
		isConn=FALSE;
		return TRUE;
	}
	catch(_com_error &e)
	{
		LOG("[Db][%s]", (char*)e.Description());
		return FALSE;
	}
}

BOOL CAdo::ExecuteNonQuery(char * sql)
{
	int i = 0;
	try
	{
		command->CommandText = sql;
		command->Execute(NULL, NULL, adCmdText);
		return TRUE;
	}
	catch (_com_error &e)
	{
		LOG("[Db][%s]", (char*)e.Description());
		return FALSE;
	}
}
BOOL CAdo::ExecuteNonQuery(char * * sql,int Count)
{
	int i=0;
	try
	{
		//开始事务
		conn->BeginTrans();
		//将SQL添加到command中，然后提交事务
		for(i=0;i<Count;i++)
		{
			command->CommandText=sql[i];
			command->Execute(NULL,NULL,adCmdText);
		}
		conn->CommitTrans();
		return TRUE;
	}
	catch(_com_error &e)
	{
		//回滚事务
		conn->RollbackTrans();
		//关闭连接
		//Close();
		LOG("[Db][%s]", (char*)e.Description());
		return FALSE;
	}
}
BOOL CAdo::ExecuteSelect(char *sql)
{
  //执行查询，得到rs
  try
  {
	  command->CommandText=sql;
	  rs=command->Execute(NULL,NULL,adCmdText);
	  return TRUE;
  }
  catch(_com_error &e)
  {
	  //Close();
	  LOG("[Db][%s]", (char*)e.Description());
	  return FALSE;
  }
}
//准备在哪张表里面添加一行数据
BOOL CAdo::AddNew(char * tableName)
{
	BOOL isSuc=FALSE;
	try
	{
		CString sql;
		sql.Format("Select * FROM %s",tableName);
		//打开记录集
        rs->Open(sql.GetBuffer(sql.GetLength()),conn.GetInterfacePtr(),
			    adOpenDynamic,adLockPessimistic,adCmdText);
		//添加一行
        rs->AddNew();
		isSuc=TRUE;
	}
	catch(_com_error &e)
	{
		//Close();
		LOG("[Db][%s]", (char*)e.Description());
		isSuc=FALSE;
	}
	return isSuc;
}
//添加非二进制字段的值
BOOL CAdo::BindField(char * fieldName,_variant_t value)
{
	BOOL isSuc=FALSE;
	try
	{
		//添加一个字段
		rs->PutCollect(fieldName,value);
		isSuc=TRUE;
	}
	catch(_com_error &e)
	{
		//Close();
		LOG("[Db][%s]", (char*)e.Description());
		isSuc=FALSE;
	}
	return isSuc;
}
//添加二进制或大文本字段的值
BOOL CAdo::BindField(char * fieldName,char * buf,int len)
{
	BOOL isSuc=FALSE;
	VARIANT		varBLOB;
	SAFEARRAY	*psa;
	SAFEARRAYBOUND	rgsabound[1];
	try
	{
		do
		{
			if(buf==NULL)
			{
				isSuc=FALSE;
				break;
			}
			rgsabound[0].lLbound = 0;
			rgsabound[0].cElements = len;
			psa = SafeArrayCreate(VT_UI1, 1, rgsabound); //创建SAFEARRAY对象
			for (long i = 0; i < (long)len; i++)
				SafeArrayPutElement (psa, &i, buf++);   //将buf指向的二进制数据保存到SAFEARRAY对象psa中
			varBLOB.vt = VT_ARRAY | VT_UI1;             //将varBLOB的类型设置为BYTE类型的数组
			varBLOB.parray = psa;                       //为varBLOB变量赋值
			rs->GetFields()->GetItem(fieldName)->AppendChunk(varBLOB);///加入BLOB类型的数据
			isSuc=TRUE;
		}while(0);
	}
	catch(_com_error &e)
	{
		//Close();
		LOG("[Db][%s]", (char*)e.Description());
		isSuc=FALSE;
	}
	return isSuc;
}
//确认添加
BOOL CAdo::Update()
{
	BOOL isSuc=FALSE;
	try
	{
		//更新，提交行
        rs->Update();
		isSuc=TRUE;
	}
	catch(_com_error &e)
	{
		//Close();
		LOG("[Db][%s]", (char*)e.Description());
		isSuc=FALSE;
	}
	return isSuc;
}

BOOL CAdo::MoveNext()
{
	//移动记录集到下一个位置
	try
	{
		rs->MoveNext();
		if(rs->ADOEOF)
			return FALSE;
		else
			return TRUE;
	}catch(_com_error &e)
	{
		LOG("[Db][%s]", (char*)e.Description());
		return FALSE;
	}
}
BOOL CAdo::MoveFirst()
{
     if(rs->State)
		 rs->MoveFirst();
	 return TRUE;
}
BOOL CAdo::MoveLast()
{
	if(rs->State)
		rs->MoveLast();
	return TRUE;
}
BOOL CAdo::MovePrevious()
{
	if(rs->State)
		rs->MovePrevious();
	return TRUE;
}
BOOL CAdo::GetBoolValue(char * itemName)
{
	BOOL bRet=FALSE;
	_variant_t v;
	try
	{
		v=rs->GetCollect(itemName);
		bRet=v.boolVal;
		return bRet;
	}
	catch(_com_error &e)
	{
		LOG("[Db][%s]", (char*)e.Description());
		return FALSE;
	}
}
unsigned int CAdo::GetUIntValue(char *itemName)
{
	unsigned int bUint;
	_variant_t v;
	try
	{
		v=rs->GetCollect(itemName);
		bUint=v.uintVal;
		return bUint;
	}
	catch(_com_error &e)
	{
		LOG("[Db][%s]", (char*)e.Description());
		return 0;
	}
}
int CAdo::GetIntValue(char *itemName)
{
    int iRet;
	_variant_t v;
	try
	{
		v=rs->GetCollect(itemName);
		iRet=v.intVal;
		return iRet;
	}
	catch(_com_error &e)
	{
		LOG("[Db][%s]", (char*)e.Description());
		return 0;
	}
}
unsigned long CAdo::GetULongValue(char * itemName)
{
	 unsigned long uLRet;
	_variant_t v;
	try
	{
		v=rs->GetCollect(itemName);
		uLRet=v.ulVal;
		return uLRet;
	}
	catch(_com_error &e)
	{
		LOG("[Db][%s]", (char*)e.Description());
		return 0;
	}
}
long CAdo::GetLongValue(char * itemName)
{
	 long lRet;
	_variant_t v;
	try
	{
		v=rs->GetCollect(itemName);
		lRet=v.lVal;
		return lRet;
	}
	catch(_com_error &e)
	{
		LOG("[Db][%s]", (char*)e.Description());
		return 0;
	}
}
double CAdo::GetDoubleValue(char * itemName)
{
	double dRet;
	_variant_t v;
	try
	{
		v=rs->GetCollect(itemName);
		dRet=v.dblVal;
		return dRet;
	}
	catch(_com_error &e)
	{
		LOG("[Db][%s]", (char*)e.Description());
		return 0.0;
	}
}
float CAdo::GetFloatValue(char * itemName)
{
	float fRet;
	_variant_t v;
	try
	{
		v=rs->GetCollect(itemName);
		fRet=v.fltVal;
		return fRet;
	}
	catch(_com_error &e)
	{
		LOG("[Db][%s]", (char*)e.Description());
		return 0.0;
	}
}
BOOL CAdo::GetStringValue(char * itemName,char *value)
{
	_variant_t v;
	try
	{
		v=rs->GetCollect(itemName);
		strcpy(value,(char*)_bstr_t(v));
		return TRUE;
	}
	catch(_com_error &e)
	{
		LOG("[Db][%s]", (char*)e.Description());
		return FALSE;
	}
}
//获得数据
int CAdo::GetBLOBValue(char * itemName,char * pBuf)
{
	_variant_t varBLOB;
	long lBufLen=0;
	char * p=NULL;
	try
	{
		do
		{
			lBufLen = rs->GetFields()->GetItem(itemName)->ActualSize;///得到数据的长度
			//如果缓冲区为空，则只获得长度
			if(pBuf==NULL)break;
			varBLOB = rs->GetFields()->GetItem(itemName)->GetChunk(lBufLen);//获得数据
			if(varBLOB.vt == (VT_ARRAY | VT_UI1))//判断数据类型是否正确
			{
				SafeArrayAccessData(varBLOB.parray,(void **)&p);//得到指向数据的指针
				::memcpy(pBuf,p,lBufLen);
				SafeArrayUnaccessData (varBLOB.parray);
			}	
		}while(0);
	}
	catch(_com_error &e)
	{
		LOG("[Db][%s]", (char*)e.Description());
		lBufLen=0;
	}
	return (int)lBufLen;
}

