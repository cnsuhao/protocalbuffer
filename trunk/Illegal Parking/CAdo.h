#import "C:\Program Files\Common Files\System\ado\msado15.dll" rename("EOF","ADOEOF") no_namespace 

/********************************************************
 * 类名称   ：CAdo                                      *
 * 类作用   ：用ADO方式连接数据库，并向数据库传递SQL语句*
 *             来操作数据库。                           *
 * 作者     ：沈毅                                      *
 * 时间     ：2007/10/28                                *
 * 第一次修改：                                         *
 * 时间     ：2007/10/29                                *
 * 修改内容 ：                                          *
 *          1.增加了MoveFirst，MoveLast,MovePreviou函数 *
 *          2.测试了select * from tablename语句         *
 *          3.测试了select count(*) from tablename语句  *
 *          4.测试了insert语句                          *
 *          5.测试了Get***Value系列函数的返回值         *
 * 第二次修改：                                         *
 * 时间      ：2008/3/11                                *
 * 修改内容  ：                                         *
 *          1. 增加了AddNew,BindField和Update三个函数   *
 *          2. 增加了GetBLOBValue函数                   *
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
	//属性，表示是否处于连接状态
	BOOL isConn;

	CAdo();
	/******************************************************************************************
	 * 函数名称  ： 构造函数                                                                  *
	 * 参数说明  ：                                                                           *
	 *   1. connStr：连接字符串，此参数不能为空，样式如下：                                   *
	 * Provider=Microsoft.Jet.OLEDB.4.0;User ID=Admin;Data Source=E:\\代码库\\student.mdb"    *
	 *   2. userName:用户名                                                                   *
	 *   3. passWord：密码                                                                    *
	 ******************************************************************************************/
	CAdo(char *connStr,char *userName=NULL,char *passWord=NULL);
	~CAdo();
	//连接数据库
	BOOL Connect(char *connStr,char *userName=NULL,char *passWord=NULL);
	//关闭数据库
	BOOL Close();
	/******************************************************************************
	 * 函数名称  ：ExecuteNonQuery                                                *
	 * 函数作用  ：执行非select的SQL语句，包括insert,update,delete,create,droup等 *
	 *             此函数内部采用事务执行方式。                                   *
	 * 函数参数  ：                                                               *
	 *    1. sql : 一个二维字符串数组，字符串数组中的每一项均为一个SQL语句        *
	 *       定义形式如下：                                                       *
	 *                      char * sql[10];                                       *
	 *                      sql[0]="delete from tablename";                       *
	 *                      sql[1]="update table******   ";                       *
	 *                      ......                                                *
	 *                      ExecuteNonQuery(sql,10)                               *
	 *   2. Count :表示前面的二维字符串数组有多少项，从0开始                      *
	 ******************************************************************************/
	BOOL ExecuteNonQuery(char *  sql);
	//执行非查询语句，使用事务处理方式，sql是一个SQL语句的字符串数组，Count表示前面的数组有多少项
	BOOL ExecuteNonQuery(char * * sql,int Count);
	//执行查询的SQL，本类中没有提供获得记录集个数的函数，因此只能通过
	//执行select count(*) as cnt from tablename where *****来获得记录集
    BOOL ExecuteSelect(char *sql);

	//*********************************************************************
	//以下三个函数是为二进制对象(BLOB),添加的，可以用来存储有二进制数据的行
	//准备在哪张表里面添加一行数据
	BOOL AddNew(char * tableName);
	//添加非二进制字段的值
	//调用形式：BindField("name",_variant_t("yshen"));
	//或BindField("old",_variant_t((long)25));
	BOOL BindField(char * fieldName,_variant_t value);
	//添加二进制字段的值
	BOOL BindField(char * fieldName,char * buf,int len);
	//确认添加
	BOOL Update();
	//**********************************************************************

    //以下是一组从rs中获得数据的函数
	BOOL GetBoolValue(char * itemName);
	unsigned int GetUIntValue(char *itemName);
	int GetIntValue(char *itemName);
	unsigned long GetULongValue(char * itemName);
	long GetLongValue(char * itemName);
	double GetDoubleValue(char * itemName);
	float GetFloatValue(char * itemName);
	BOOL GetStringValue(char * itemName,char* value);
	//获得二进制数据或大文本缓冲区地址,如果pBuf==NULL,则获取长度
	int GetBLOBValue(char * itemName,char * pBuf);

    //操作记录集的函数
	//移动记录集到第一条记录
	BOOL MoveFirst();
	//移动记录集到下一条记录，为空是返回FALSE
	BOOL MoveNext();
	//移动记录集到最后一条记录
    BOOL MoveLast();
	BOOL MovePrevious();
};