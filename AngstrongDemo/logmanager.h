#pragma once
#include <fstream>

enum ELogLevel
{
	ELogLevel_System = 0,
	ELogLevel_Administor,
	ELogLevel_Operator,
	ELogLevel_User,
	ELogLevel_Count,
};
enum ELogType
{
	ELogType_Running = 0,
	ELogType_Debug,
	ELogType_Error,
	ELogType_Count,
};

class LogManager
{
public:
	static LogManager *GetInstance();

	static void Write(std::string msg, ELogLevel log_level = ELogLevel::ELogLevel_User, ELogType log_type = ELogType::ELogType_Running);
private:
	LogManager();
	virtual ~LogManager();
private:
	static LogManager *log_manager_;
	static std::ofstream *out_file_;
};

