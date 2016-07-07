#import "C:\Program Files\Common Files\System\ado\msado15.dll" rename("EOF","ADOEOF") no_namespace 

/********************************************************
 * ������   ��CAdo                                      *
 * ������   ����ADO��ʽ�������ݿ⣬�������ݿ⴫��SQL���*
 *             ���������ݿ⡣                           *
 * ����     ������                                      *
 * ʱ��     ��2007/10/28                                *
 * ��һ���޸ģ�                                         *
 * ʱ��     ��2007/10/29                                *
 * �޸����� ��                                          *
 *          1.������MoveFirst��MoveLast,MovePreviou���� *
 *          2.������select * from tablename���         *
 *          3.������select count(*) from tablename���  *
 *          4.������insert���                          *
 *          5.������Get***Valueϵ�к����ķ���ֵ         *
 * �ڶ����޸ģ�                                         *
 * ʱ��      ��2008/3/11                                *
 * �޸�����  ��                                         *
 *          1. ������AddNew,BindField��Update��������   *
 *          2. ������GetBLOBValue����                   *
 ********************************************************/

class CAdo
{
private:
	HRESULT hr;
	_ConnectionPtr  conn;
    _CommandPtr  command;
    _RecordsetPtr  rs;
protected:

public :
	//���ԣ���ʾ�Ƿ�������״̬
	BOOL isConn;

	CAdo();
	/******************************************************************************************
	 * ��������  �� ���캯��                                                                  *
	 * ����˵��  ��                                                                           *
	 *   1. connStr�������ַ������˲�������Ϊ�գ���ʽ���£�                                   *
	 * Provider=Microsoft.Jet.OLEDB.4.0;User ID=Admin;Data Source=E:\\�����\\student.mdb"    *
	 *   2. userName:�û���                                                                   *
	 *   3. passWord������                                                                    *
	 ******************************************************************************************/
	CAdo(char *connStr,char *userName=NULL,char *passWord=NULL);
	~CAdo();
	//�������ݿ�
	BOOL Connect(char *connStr,char *userName=NULL,char *passWord=NULL);
	//�ر����ݿ�
	BOOL Close();
	/******************************************************************************
	 * ��������  ��ExecuteNonQuery                                                *
	 * ��������  ��ִ�з�select��SQL��䣬����insert,update,delete,create,droup�� *
	 *             �˺����ڲ���������ִ�з�ʽ��                                   *
	 * ��������  ��                                                               *
	 *    1. sql : һ����ά�ַ������飬�ַ��������е�ÿһ���Ϊһ��SQL���        *
	 *       ������ʽ���£�                                                       *
	 *                      char * sql[10];                                       *
	 *                      sql[0]="delete from tablename";                       *
	 *                      sql[1]="update table******   ";                       *
	 *                      ......                                                *
	 *                      ExecuteNonQuery(sql,10)                               *
	 *   2. Count :��ʾǰ��Ķ�ά�ַ��������ж������0��ʼ                      *
	 ******************************************************************************/
	BOOL ExecuteNonQuery(char *  sql);
	//ִ�зǲ�ѯ��䣬ʹ��������ʽ��sql��һ��SQL�����ַ������飬Count��ʾǰ��������ж�����
	BOOL ExecuteNonQuery(char * * sql,int Count);
	//ִ�в�ѯ��SQL��������û���ṩ��ü�¼�������ĺ��������ֻ��ͨ��
	//ִ��select count(*) as cnt from tablename where *****����ü�¼��
    BOOL ExecuteSelect(char *sql);

	//*********************************************************************
	//��������������Ϊ�����ƶ���(BLOB),��ӵģ����������洢�ж��������ݵ���
	//׼�������ű��������һ������
	BOOL AddNew(char * tableName);
	//��ӷǶ������ֶε�ֵ
	//������ʽ��BindField("name",_variant_t("yshen"));
	//��BindField("old",_variant_t((long)25));
	BOOL BindField(char * fieldName,_variant_t value);
	//��Ӷ������ֶε�ֵ
	BOOL BindField(char * fieldName,char * buf,int len);
	//ȷ�����
	BOOL Update();
	//**********************************************************************

    //������һ���rs�л�����ݵĺ���
	BOOL GetBoolValue(char * itemName);
	unsigned int GetUIntValue(char *itemName);
	int GetIntValue(char *itemName);
	unsigned long GetULongValue(char * itemName);
	long GetLongValue(char * itemName);
	double GetDoubleValue(char * itemName);
	float GetFloatValue(char * itemName);
	BOOL GetStringValue(char * itemName,char* value);
	//��ö��������ݻ���ı���������ַ,���pBuf==NULL,���ȡ����
	int GetBLOBValue(char * itemName,char * pBuf);

    //������¼���ĺ���
	//�ƶ���¼������һ����¼
	BOOL MoveFirst();
	//�ƶ���¼������һ����¼��Ϊ���Ƿ���FALSE
	BOOL MoveNext();
	//�ƶ���¼�������һ����¼
    BOOL MoveLast();
	BOOL MovePrevious();
};