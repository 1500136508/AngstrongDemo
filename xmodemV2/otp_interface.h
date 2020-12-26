#pragma once
#include "data_type.h"
#include <string>

#define TYPE_D 0
#define TYPE_P 1
#define TYPE_T 2

 class __declspec(dllexport) com_controler {
 private:
	 st_app app;
	 u8 buf[64], sum, x;
	 u32 cnt, tmr, retLen;
	 char response[64];
	 unsigned char checkCode[4];
	 int readLen = 0;
	 unsigned char *datBase;
	 int datLen = 0;
	 int get_response();
 public:
	int use_comm_handle(HANDLE handle);
	int get_comm_handle(HANDLE &handle);
	int init_comm(int com_index);
	int set_vcsel(bool turn_on);
	int set_irled(bool turn_on);
	int set_proximity(bool turn_on);
	int get_temperature(float &temperature);
	int set_vcsel_current(unsigned short slave_current);
	int set_vcsel_duty(unsigned short slave_duty);
	int set_inf_current(unsigned short slave_current);
	int set_inf_duty(unsigned short slave_duty);

	int get_status(int &irled_status, int &vcsel_status, int &proximity_status, int &ITO_status,
		int &currentVCSEL, int &dutyVCSEL, int &currentINF, int &dutyINF, int &DIST, int &ITOVAL, float &temperature);
	int get_flash_id(char* flash_id);
	int upload_data_to_flash(unsigned char* data, int data_lenght);
	//int upload_data_to_flash_test(unsigned char* data, int data_lenght);
	int download_data_from_flash(unsigned char* data, int data_lenght);
	int upload_data_to_flash_type(unsigned char* data, int data_lenght, int type);
	//int download_data_from_flash(unsigned char* data, int data_lenght);
	int sendDataFix(std::string data, std::string want);
	int close_comm();

	int write_comm(std::string com,std::string &info);
	int readCom(std::string &data, int &datalen, int waitTime);
	int readUtilWant(std::string want, int waitTime);
};