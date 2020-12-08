#include "ConfigInstance.h"
#include <windows.h>
#include <sstream>
#include <fstream>
#include <io.h>

ConfigInstace::ConfigInstace(const std::string &strPath)
	: m_strPath(strPath)
{
	
}

ConfigInstace::~ConfigInstace()
{

}

std::string ConfigInstace::GetString(const std::string &strSection, const std::string &strKey, const std::string &strDefault /*= ""*/)
{
	char szBuf[255] = { 0 };
	GetPrivateProfileStringA(strSection.c_str(), strKey.c_str(), strDefault.c_str(), szBuf, 255, m_strPath.c_str());

	std::string strRet = szBuf;
	return strRet;
}

bool ConfigInstace::SetString(const std::string &strSection, const std::string &strKey, const std::string &strValue)
{
	bool bRet = WritePrivateProfileStringA(strSection.c_str(), strKey.c_str(), strValue.c_str(), m_strPath.c_str());
	return bRet;
}

int ConfigInstace::GetInt(const std::string &strSection, const std::string &strKey, const int nDefault /*= 0*/)
{
	return GetPrivateProfileIntA(strSection.c_str(), strKey.c_str(), nDefault, m_strPath.c_str());
}

bool ConfigInstace::SetInt(const std::string &strSection, const std::string &strKey, const int nValue)
{
	std::stringstream inStream;
	inStream << nValue;
	return SetString(strSection, strKey, inStream.str());
}
