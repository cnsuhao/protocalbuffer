// CAdoOperate.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "CAdo.h"
#include "LogFile.h"

CAdo::CAdo()
{
	isConn=FALSE;
	//��ʼ��COM���
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
	//���isConn��TRUE����ʾ�Ѿ����ӹ���
	if(isConn)
		return TRUE;
	//��������ַ���Ϊ�գ���ֱ�ӷ���ʧ��
	if(connStr==NULL)
		return FALSE;
	try
	{
		//��������
		conn.CreateInstance(__uuidof(Connection));
		conn->Open(connStr,userName,passWord,-1);
		//��������
		command.CreateInstance(__uuidof(Command));
		command->ActiveConnection=conn;
		//���������
		 rs.CreateInstance(__uuidof(Recordset));
		 //��ʾ����������
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
	//����Ѿ��ص���
	if(!isConn)
		return TRUE;
	//���򣬹ر����ݿ�����
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
		//��ʼ����
		conn->BeginTrans();
		//��SQL��ӵ�command�У�Ȼ���ύ����
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
		//�ع�����
		conn->RollbackTrans();
		//�ر�����
		//Close();
		LOG("[Db][%s]", (char*)e.Description());
		return FALSE;
	}
}
BOOL CAdo::ExecuteSelect(char *sql)
{
  //ִ�в�ѯ���õ�rs
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
//׼�������ű��������һ������
BOOL CAdo::AddNew(char * tableName)
{
	BOOL isSuc=FALSE;
	try
	{
		CString sql;
		sql.Format("Select * FROM %s",tableName);
		//�򿪼�¼��
        rs->Open(sql.GetBuffer(sql.GetLength()),conn.GetInterfacePtr(),
			    adOpenDynamic,adLockPessimistic,adCmdText);
		//���һ��
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
//��ӷǶ������ֶε�ֵ
BOOL CAdo::BindField(char * fieldName,_variant_t value)
{
	BOOL isSuc=FALSE;
	try
	{
		//���һ���ֶ�
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
//��Ӷ����ƻ���ı��ֶε�ֵ
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
			psa = SafeArrayCreate(VT_UI1, 1, rgsabound); //����SAFEARRAY����
			for (long i = 0; i < (long)len; i++)
				SafeArrayPutElement (psa, &i, buf++);   //��bufָ��Ķ��������ݱ��浽SAFEARRAY����psa��
			varBLOB.vt = VT_ARRAY | VT_UI1;             //��varBLOB����������ΪBYTE���͵�����
			varBLOB.parray = psa;                       //ΪvarBLOB������ֵ
			rs->GetFields()->GetItem(fieldName)->AppendChunk(varBLOB);///����BLOB���͵�����
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
//ȷ�����
BOOL CAdo::Update()
{
	BOOL isSuc=FALSE;
	try
	{
		//���£��ύ��
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
	//�ƶ���¼������һ��λ��
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
//�������
int CAdo::GetBLOBValue(char * itemName,char * pBuf)
{
	_variant_t varBLOB;
	long lBufLen=0;
	char * p=NULL;
	try
	{
		do
		{
			lBufLen = rs->GetFields()->GetItem(itemName)->ActualSize;///�õ����ݵĳ���
			//���������Ϊ�գ���ֻ��ó���
			if(pBuf==NULL)break;
			varBLOB = rs->GetFields()->GetItem(itemName)->GetChunk(lBufLen);//�������
			if(varBLOB.vt == (VT_ARRAY | VT_UI1))//�ж����������Ƿ���ȷ
			{
				SafeArrayAccessData(varBLOB.parray,(void **)&p);//�õ�ָ�����ݵ�ָ��
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

