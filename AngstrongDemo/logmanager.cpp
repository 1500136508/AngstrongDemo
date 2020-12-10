#pragma execution_character_set("utf-8")
#include "logmanager.h"
#include <iostream>
#include <windows.h>
#include <string>
#include <QDir>
#include "outputview.h"

LogManager *LogManager::log_manager_ = new LogManager();
std::ofstream *LogManager::out_file_ = nullptr;
LogManager::LogManager()
{
	if (!out_file_)
	{
		QDir qd;
		std::string folder_path = "Log";
		qd.mkpath(QString::fromStdString(folder_path));

		SYSTEMTIME system_time;
		GetLocalTime(&system_time);
		std::string log_file_name = folder_path+"/"+std::to_string(system_time.wYear)+"-"+ std::to_string(system_time.wMonth)
			+"-"+ std::to_string(system_time.wDay)+".txt";
		out_file_ = new std::ofstream(log_file_name.c_str(), std::ofstream::app);
		std::cout.rdbuf(out_file_->rdbuf());
		if (!out_file_->is_open())
		{
			std::string err_msg = "failed to open the log text!";
		}
	}
}

LogManager::~LogManager()
{
	if (log_manager_)
	{
		delete log_manager_;
		log_manager_ = nullptr;
	}
	if (out_file_)
	{
		out_file_->close();
		delete out_file_;
		out_file_ = nullptr;
	}
}

void LogManager::Write(std::string msg, ELogLevel log_level, ELogType log_type)
{
	if (!out_file_)
	{
		return;
	}

	std::string msg_prefix("");
	SYSTEMTIME system_time;
	GetLocalTime(&system_time);
	if (system_time.wSecond < 10)
	{
		msg_prefix = "[" + std::to_string(system_time.wHour) + ":" + std::to_string(system_time.wMinute)
			+ ":0" + std::to_string(system_time.wSecond) + "]";
	}
	else
	{
		msg_prefix = "[" + std::to_string(system_time.wHour) + ":" + std::to_string(system_time.wMinute)
			+ ":" + std::to_string(system_time.wSecond) + "]";
	}
	

	switch (log_level)
	{
	case ELogLevel_System:
	{
		msg_prefix = msg_prefix + " [System]";
	}
		break;
	case ELogLevel_Administor:
	{
		msg_prefix = msg_prefix + "	[Administor]";
	}
		break;
	case ELogLevel_Operator:
	{
		msg_prefix = msg_prefix + "	[Operator]";
	}
		break;
	case ELogLevel_User:
	{
		msg_prefix = msg_prefix + "	[User]";
	}
		break;
	default:
	{
		msg_prefix = msg_prefix + "	[Unknow]";
	}
		break;
	}

	switch (log_type)
	{
	case ELogType_Running:
	{
		msg_prefix = msg_prefix + " [Running]";
	}
		break;
	case ELogType_Debug:
	{
		msg_prefix = msg_prefix + " [Debug]";
	}
		break;
	case ELogType_Error:
	{
		msg_prefix = msg_prefix + " [Error]";
	}
		break;
	default:
	{
		msg_prefix = msg_prefix + " [Unknow]";
	}
		break;
	}
	std::cout <<msg_prefix<<"	"<< msg << std::endl;
	//OutputView::GetInstance()->WriteText(QString::fromStdString(msg_prefix) + QString::fromStdString(msg));
}

LogManager * LogManager::GetInstance()
{
	if (!log_manager_)
	{
		log_manager_ = new LogManager();
	}
	return log_manager_;
}
