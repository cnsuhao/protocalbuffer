#pragma once
#include <string>
#include <vector>
#include <map>

using namespace std;
typedef std::map<std::string, std::string> MAP_STRING_STRING;

class SectionLabel
{
public:
	SectionLabel();
	~SectionLabel();

	/*
	  @para strLabel: name of label which you want to read
	  @para nDefaultValue: if the specified label is not exist, then return nDefaultValue
	  return : the value of strLabel label
	*/
	int ReadIntValue(const std::string& strLabel, int nDefaultValue);

	__int64 ReadInt64Value(const std::string&strLabel, __int64 nDefaultValue);

	/*
	  the para meaning ditto
	*/
	const std::string& ReadStrValue(const std::string& strLabel, const std::string& strDefaultValue);

	void InitSection(const std::string& strParseString);
private:

	MAP_STRING_STRING m_mapSecConfInfo;
};

typedef std::map<std::string, SectionLabel*> MAP_STRING_POINTER_SectionLabel;

class ReadConfig
{
public:
	/*
	  return a static reference instance of class ReadConfig
	*/
	static ReadConfig& Instance();

	/*
	  @para strConf: name of config file which want to read 
	  return: successful is true, otherwise false
	*/
	void InitConf(const std::string& strConf);

	/*
	  @para strLabel: name of section which you interest
	  return : successful is pointer of class SectionLabel, otherwise NULL
	*/
	SectionLabel* ConfSetSection(const std::string& strSection);
private:
	ReadConfig();
	~ReadConfig();

	bool ReadConf(const std::string& strConfName);

	bool IsAnnotateOfString(std::string& strDecide);

	bool IsSectionLabelOfString(std::string& strDecide);

	std::string m_strConfigName;
	MAP_STRING_POINTER_SectionLabel m_mapFileConfInfo;

	SectionLabel m_ErrorSection;
};

