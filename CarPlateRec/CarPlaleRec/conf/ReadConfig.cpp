#include "ReadConfig.h"
#include <fstream>
#include <windows.h>


ReadConfig::ReadConfig()
{
}


ReadConfig::~ReadConfig()
{
	MAP_STRING_POINTER_SectionLabel::iterator it;
	for (it = m_mapFileConfInfo.begin(); it != m_mapFileConfInfo.end(); it++)
		delete it->second;
}

ReadConfig& ReadConfig::Instance()
{
	static ReadConfig oReadConfig;
	return oReadConfig;
}

void ReadConfig::InitConf(const std::string& strConf)
{
	if (0 != strConf.size())
		ReadConf(strConf);
}

bool ReadConfig::ReadConf(const std::string& strConfName)
{
	std::string strSectionName;
	SectionLabel *pSection = NULL;

	fstream fOpen;
	fOpen.open(strConfName.c_str());
	std::string strReadRow;
	if (fOpen.is_open())
	{
		char lastChar;
		std::string strWriteRow;

		while (getline(fOpen, strReadRow))
		{
			strWriteRow.clear();
			for (unsigned int i = 0; i < strReadRow.size(); i++)
			{
				if (('=' == strReadRow[i] && ' ' == lastChar))
				{
					strWriteRow[strWriteRow.size() - 1] = strReadRow.c_str()[i];
				}
				else if (!(' ' == strReadRow[i] && (0 == strWriteRow.size() || ' ' == lastChar || '=' == lastChar)))//delete the space which start at row or uninterrupted
					strWriteRow += strReadRow[i];
				lastChar = strReadRow[i];
			}
			if (!IsAnnotateOfString(strWriteRow)) //judge strWriteRow whether a annotate row
			{
				if (IsSectionLabelOfString(strWriteRow)) //judge strWriteRow whether a section label row
				{
					if (NULL != pSection && 0 != strWriteRow.size())
						pSection->InitSection(strWriteRow);

					strSectionName = strWriteRow.substr(1,strWriteRow.find_first_of(']') - 1);
					pSection = new SectionLabel();
					m_mapFileConfInfo.insert(pair<std::string, SectionLabel*>(strSectionName, pSection));
				}
				else
				{
					if (NULL != pSection && 0 != strWriteRow.size())
						pSection->InitSection(strWriteRow);
				}
			}
		}
		/*  
		if (NULL != pSection && 0 != strWriteRow.size()
			&& !IsAnnotateOfString(strWriteRow) && !IsSectionLabelOfString(strWriteRow))
			pSection->InitSection(strWriteRow);
		*/
		fOpen.close();
		fOpen.clear();
		return true;
	}
	else
		return false;
}

SectionLabel* ReadConfig::ConfSetSection(const std::string& strSection)
{
	MAP_STRING_POINTER_SectionLabel::iterator it;
	it = m_mapFileConfInfo.find(strSection);
	if (m_mapFileConfInfo.end() != it)
		return it->second;
	else
		return &m_ErrorSection;
}

bool ReadConfig::IsSectionLabelOfString(std::string& strDecide)
{
	return 0 == strDecide.find_first_of('[');
}

bool ReadConfig::IsAnnotateOfString(std::string& strDecide)
{
	return 0 == strDecide.find_first_of('#');
}


SectionLabel::SectionLabel()
{
	
}

SectionLabel::~SectionLabel()
{

}

void SectionLabel::InitSection(const std::string& strParseString)
{
	unsigned int nLeftValue = 0;
	unsigned int nRightValue = 0;
	unsigned int nLastRightValue = 0;

	while (nRightValue < strParseString.size() && nLeftValue < strParseString.size())
	{
		nLeftValue = strParseString.find('=', nLeftValue + 1);
		nRightValue = strParseString.find(' ', nRightValue + 1);
		if (strParseString.npos == nLeftValue)
		{
			continue;
		}
		else if (strParseString.npos == nRightValue)
			nRightValue = strParseString.size();
		std::string strLeftValue = strParseString.substr(nLastRightValue, nLeftValue - nLastRightValue);
		std::string strRightValue = strParseString.substr(nLeftValue + 1, nRightValue - nLeftValue - 1).c_str();
		m_mapSecConfInfo.insert(pair<std::string, std::string>(strLeftValue, strRightValue));
		nLastRightValue = nRightValue + 1;
	}
}

int SectionLabel::ReadIntValue(const std::string& strLabel, int nDefaultValue)
{
	MAP_STRING_STRING::iterator it;
	it = m_mapSecConfInfo.find(strLabel);
	if (it != m_mapSecConfInfo.end())
		return atoi(it->second.c_str());
	else
		return nDefaultValue;
}

const std::string& SectionLabel::ReadStrValue(const std::string& strLabel, const std::string& strDefaultValue)
{
	MAP_STRING_STRING::iterator it;
	it = m_mapSecConfInfo.find(strLabel);
	if (it != m_mapSecConfInfo.end())
		return it->second;
	else
		return strDefaultValue;
}

__int64 SectionLabel::ReadInt64Value(const std::string&strLabel, __int64 nDefaultValue)
{
	MAP_STRING_STRING::iterator it;
	it = m_mapSecConfInfo.find(strLabel);
	if (it != m_mapSecConfInfo.end())
		return atoll(it->second.c_str());
	else
		return nDefaultValue;
}

