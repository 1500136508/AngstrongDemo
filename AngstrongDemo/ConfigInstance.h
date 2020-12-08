#pragma once

#include <string>

class ConfigInstace
{
public:
	ConfigInstace(const std::string &strPath);
	~ConfigInstace();

public:
	std::string GetString(const std::string &strSection, const std::string &strKey, const std::string &strDefault = "");
	bool SetString(const std::string &strSection, const std::string &strKey, const std::string &strValue);

	int GetInt(const std::string &strSection, const std::string &strKey, const int nDefault = 0);
	bool SetInt(const std::string &strSection, const std::string &strKey, const int nValue);

private:
	std::string m_strPath;
};