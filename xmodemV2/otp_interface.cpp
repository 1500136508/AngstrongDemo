#include "otp_interface.h"
#include "utilcom.h"
#include "comm.h"
#include "xmodem.h"
#include <string>
#include <vector>

#define STRINGLIST std::vector<std::string>
STRINGLIST splitString(std::string src, char split) {
	STRINGLIST reData;
	std::string tmp = "";
	for (int i = 0; i < src.size(); i++) {
		if (src[i] != split)
			tmp += src[i];
		else {
			reData.push_back(tmp);
			tmp = "";
		}
		if (i == src.size() - 1 && tmp != "") {
			reData.push_back(tmp);
		}
	}
	return reData;
}

std::string checkNumString(std::string &numberString) {
	std::string str = "";
	for (int i = 0; i < numberString.size(); i++) {
		if ((numberString[i] >= '0' && numberString[i] <= '9') || numberString[i] == '.')
			str += numberString[i];
	}
	return str;
}

int com_controler::get_response() {
	if (__strstr(app.comm.rbuf, reinterpret_cast<const u8*>("dbgrx:ERROR")) != null)
		return c_ret_nk;

	if (retLen < 7) return c_ret_xk;

	for (x = 0; x < retLen - 6; x++)
	{
		if ((app.comm.rbuf[x + 0] == checkCode[0])
			&& (app.comm.rbuf[x + 1] == checkCode[1])
			&& (app.comm.rbuf[x + 2] == checkCode[2])
			&& (app.comm.rbuf[x + 3] == checkCode[3]))
		{
			sum = (u8)(checkCode[0] + checkCode[1] + checkCode[2] + checkCode[3] + app.comm.rbuf[x + 4] + app.comm.rbuf[x + 5]);
			if (sum == app.comm.rbuf[x + 6])
			{
				datBase = &app.comm.rbuf[x + 4];
				datLen = 2;
				return c_ret_ok;
			}
			return c_ret_nk;
		}
	}
	return c_ret_xk;
}

int com_controler::init_comm(int com_index) {
	tick_get();
	comm_init(&app);
	int retry = 0;
	while (app.comm.open == 0 && retry < 3)
	{
		comm_open(com_index, &app);
		Sleep(100);
		retry++;
	}
	Sleep(100);
	if (app.comm.open == 1)
		return c_ret_ok;
	else
		return c_ret_nk;
}

int com_controler::set_vcsel(bool turn_on) {
	comm_rx_clr(&app);
	app.slave_vcsel_onoff = '\x00';
	if (turn_on)
		app.slave_vcsel_onoff = '\x01';
	__memcpy(buf, reinterpret_cast<const u8*>("\xaa\xaf\x03\x01\x00"), 5);
	buf[4] = app.slave_vcsel_onoff;
	for (sum = x = 0; x < 5; x++)
		sum += buf[x];
	buf[x++] = sum;
	int ret = c_ret_nk;
	int retry = 0;
	if (comm_send(buf, x, &app) != c_ret_ok) return c_ret_nk;
	{
		tmr = tick_get();
		while (1)
		{
			if (tick_cmp(tmr, 60) == c_ret_ok) break;
			if (comm_read(&retLen, &app) == c_ret_ok)
				return c_ret_ok;
		}
	}
	return ret;
}

int com_controler::set_irled(bool turn_on) {
	comm_rx_clr(&app);
	app.slave_irled_onoff = '\x00';
	if (turn_on)
		app.slave_irled_onoff = '\x01';
	__memcpy(buf, reinterpret_cast<const u8*>("\xaa\xaf\x02\x01\x00"), 5);
	buf[4] = app.slave_irled_onoff;
	for (sum = x = 0; x < 5; x++)
		sum += buf[x];
	buf[x++] = sum;
	int ret = c_ret_nk;
	int retry = 0;
	if (comm_send(buf, x, &app)!=c_ret_ok) return c_ret_nk;
	{
		tmr = tick_get();
		while (1)
		{
			if (tick_cmp(tmr, 60) == c_ret_ok) break;
			if (comm_read(&retLen, &app) == c_ret_ok)
				return c_ret_ok;
		}
	}
	return ret;
}

int com_controler::set_proximity(bool turn_on) {
	comm_rx_clr(&app);
	u8 slave_proximity_onoff = '\x00';
	if (turn_on)
		slave_proximity_onoff = '\x01';
	__memcpy(buf, reinterpret_cast<const u8*>("\xaa\xaf\x0a\x01\x00"), 5);
	buf[4] = slave_proximity_onoff;
	for (sum = x = 0; x < 5; x++)
		sum += buf[x];
	buf[x++] = sum;
	int ret = c_ret_nk;
	int retry = 0;
	if (comm_send(buf, x, &app) != c_ret_ok) return c_ret_nk;
	{
		tmr = tick_get();
		while (1)
		{
			if (tick_cmp(tmr, 60) == c_ret_ok) break;
			if (comm_read(&retLen, &app) == c_ret_ok)
				return c_ret_ok;
		}
	}
	return ret;
}

int com_controler::get_temperature(float &temperature) {
	comm_rx_clr(&app);
	__memcpy(buf, reinterpret_cast<const u8*>("\xaa\xaf\x01\x01\x00"), 5);
	for (sum = x = 0; x < 5; x++)
		sum += buf[x];
	buf[x++] = sum;
	int ret = c_ret_nk;
	int retry = 0;
	if (comm_send(buf, x, &app)!=c_ret_ok) return c_ret_nk;
	{
		tmr = tick_get();
		while (1)
		{
			if (tick_cmp(tmr, 60) == c_ret_ok) break;
			if (comm_read(&retLen, &app) == c_ret_ok)
			{
				checkCode[0] = 0xaa;
				checkCode[1] = 0xaa;
				checkCode[2] = 0x03;
				checkCode[3] = 0x02;

				if (get_response() == c_ret_ok)
				{
					u8 upNum = datBase[0];
					u8 downNum = datBase[1];
					sprintf(response, "%s%d.%d\n", (upNum >= 0x80) ? "-" : "+", upNum & 0x7f, downNum);
					//std::string temper(response);
					temperature = float(atof(response));
					return c_ret_ok;
				}
				return c_ret_nk;
			}
		}
		
	}
	return c_ret_tmo;

}

int com_controler::set_vcsel_current(unsigned short slave_current) {
	comm_rx_clr(&app);
	app.slave_current = slave_current;
	__memcpy(buf, reinterpret_cast<const u8*>("\xaa\xaf\x04\x02\x00\x00"), 6);
	buf[4] = app.slave_current / 256;
	buf[5] = app.slave_current & 0xff;
	for (sum = x = 0; x < 6; x++)
		sum += buf[x];
	buf[x++] = sum;
	int ret = c_ret_nk;
	int retry = 0;
	if(comm_send(buf, x, &app)!=c_ret_ok) return c_ret_nk;
	{
		tmr = tick_get();
		while (1)
		{
			if (tick_cmp(tmr, 60) == c_ret_ok) break;
			if (comm_read(&retLen, &app) == c_ret_ok)
			{
				checkCode[0] = 0xaa;
				checkCode[1] = 0xaa;
				checkCode[2] = 0x06;
				checkCode[3] = 0x04;
				if (get_response() == c_ret_ok)
				{
					if (datBase[1]==1)
						return c_ret_ok;
				}
				return c_ret_nk;
			}
		}

	}
	return c_ret_tmo;
}

int com_controler::set_vcsel_duty(unsigned short slave_duty) {
	comm_rx_clr(&app);
	app.slave_duty = slave_duty;
	__memcpy(buf, reinterpret_cast<const u8*>("\xaa\xaf\x05\x02\x00\x00"), 6);
	buf[4] = app.slave_duty / 256;
	buf[5] = app.slave_duty & 0xff;
	for (sum = x = 0; x < 6; x++)
		sum += buf[x];
	buf[x++] = sum;
	int ret = c_ret_nk;
	int retry = 0;
	if(comm_send(buf, x, &app)!=c_ret_ok) return c_ret_nk;
	{
		tmr = tick_get();
		while (1)
		{
			if (tick_cmp(tmr, 60) == c_ret_ok) break;
			if (comm_read(&retLen, &app) == c_ret_ok)
			{
				checkCode[0] = 0xaa;
				checkCode[1] = 0xaa;
				checkCode[2] = 0x06;
				checkCode[3] = 0x04;
				if (get_response() == c_ret_ok)
				{
					if (datBase[1] == 1)
						return c_ret_ok;
				}
				return c_ret_nk;
			}
		}

	}
	return c_ret_tmo;
}

int com_controler::set_inf_current(unsigned short slave_current) {
	comm_rx_clr(&app);
	app.slave_current = slave_current;
	__memcpy(buf, reinterpret_cast<const u8*>("\xaa\xaf\x0C\x02\x00\x00"), 6);
	buf[4] = app.slave_current / 256;
	buf[5] = app.slave_current & 0xff;
	for (sum = x = 0; x < 6; x++)
		sum += buf[x];
	buf[x++] = sum;
	int ret = c_ret_nk;
	int retry = 0;
	if (comm_send(buf, x, &app) != c_ret_ok) return c_ret_nk;
	{
		tmr = tick_get();
		while (1)
		{
			if (tick_cmp(tmr, 60) == c_ret_ok) break;
			if (comm_read(&retLen, &app) == c_ret_ok)
			{
				checkCode[0] = 0xaa;
				checkCode[1] = 0xaa;
				checkCode[2] = 0x06;
				checkCode[3] = 0x04;
				if (get_response() == c_ret_ok)
				{
					if (datBase[1] == 1)
						return c_ret_ok;
				}
				return c_ret_nk;
			}
		}

	}
	return c_ret_tmo;
}

int com_controler::set_inf_duty(unsigned short slave_duty) {
	comm_rx_clr(&app);
	app.slave_duty = slave_duty;
	__memcpy(buf, reinterpret_cast<const u8*>("\xaa\xaf\x0D\x02\x00\x00"), 6);
	buf[4] = app.slave_duty / 256;
	buf[5] = app.slave_duty & 0xff;
	for (sum = x = 0; x < 6; x++)
		sum += buf[x];
	buf[x++] = sum;
	int ret = c_ret_nk;
	int retry = 0;
	if (comm_send(buf, x, &app) != c_ret_ok) return c_ret_nk;
	{
		tmr = tick_get();
		while (1)
		{
			if (tick_cmp(tmr, 60) == c_ret_ok) break;
			if (comm_read(&retLen, &app) == c_ret_ok)
			{
				checkCode[0] = 0xaa;
				checkCode[1] = 0xaa;
				checkCode[2] = 0x06;
				checkCode[3] = 0x04;
				if (get_response() == c_ret_ok)
				{
					if (datBase[1] == 1)
						return c_ret_ok;
				}
				return c_ret_nk;
			}
		}

	}
	return c_ret_tmo;
}

int com_controler::get_status(int &irled_status, int &vcsel_status, int &proximity_status, int &ITO_status,
	int &currentVCSEL, int &dutyVCSEL, int &currentINF, int &dutyINF, int &DIST, int &ITOVAL, float &temperature) {
	//irled_status = vcsel_status = proximity_status = ITO_status =  current = duty = DIST = ITOVAL = temperature = 0;
	comm_rx_clr(&app);
	__memcpy(buf, reinterpret_cast<const u8*>("\xaa\xaf\x06\x01\x00"), 5);
	for (sum = x = 0; x < 5; x++)
		sum += buf[x];
	buf[x++] = sum;
	int ret = c_ret_nk;
	int retry = 0;
	if(comm_send(buf, x, &app)!=c_ret_ok) return c_ret_nk;
	{
		tmr = tick_get();
		while (1)
		{
			if (tick_cmp(tmr, 60) == c_ret_ok) break;
			if (comm_read(&retLen,&app) == c_ret_ok)
			{
				for (x = 0; x < retLen; x++) {
					if (app.comm.rbuf[x] == 0xaa && app.comm.rbuf[x+1] == 0xaa && app.comm.rbuf[x+2] == 0x05) {
						sum = 0;
						for (int i = 0; i < retLen - 1; i++) {
							sum += app.comm.rbuf[i];
						}
						if (sum != app.comm.rbuf[retLen - 1]) return c_ret_nk;

						unsigned char* usefuldata = &app.comm.rbuf[x + 5];
						std::string dataStr(reinterpret_cast<const char*>(usefuldata), retLen - 7);
						STRINGLIST datalist = splitString(dataStr, ',');
						if (datalist.size() < 11) return c_ret_nk;
						if (datalist[0].find("ON") != std::string::npos) vcsel_status = 1; else vcsel_status = 0;
						if (datalist[1].find("ON") != std::string::npos) irled_status = 1; else irled_status = 0;
						if (datalist[2].find("ON") != std::string::npos) proximity_status = 1; else proximity_status = 0;
						if (datalist[3].find("ON") != std::string::npos) ITO_status = 1; else ITO_status = 0;
						STRINGLIST subData = splitString(datalist[4], ':');
						std::string intData = checkNumString(subData[1]);
						currentVCSEL = atoi(intData.c_str());
						subData = splitString(datalist[5], ':');
						intData = checkNumString(subData[1]);
						dutyVCSEL = atoi(intData.c_str());
						subData = splitString(datalist[6], ':');
						intData = checkNumString(subData[1]);
						currentINF = atoi(intData.c_str());
						subData = splitString(datalist[7], ':');
						intData = checkNumString(subData[1]);
						dutyINF = atoi(intData.c_str());
						subData = splitString(datalist[8], ':');
						intData = checkNumString(subData[1]);
						DIST = atoi(intData.c_str());
						subData = splitString(datalist[9], ':');
						intData = checkNumString(subData[1]);
						ITOVAL = atoi(intData.c_str());
						subData = splitString(datalist[10], ':');
						temperature = atof(checkNumString(subData[1]).c_str());
						return c_ret_ok;
					}
				}
				
				
				return c_ret_nk;
			}
		}
	}
	return c_ret_tmo;
}

int com_controler::get_flash_id(char* flash_id) {
	comm_rx_clr(&app);
	__memcpy(buf, reinterpret_cast<const u8*>("\xaa\xaf\x08\x01\x00"), 5);
	for (sum = x = 0; x < 5; x++)
		sum += buf[x];
	buf[x++] = sum;
	int ret = c_ret_nk;
	int retry = 0;
	if(comm_send(buf, x, &app) !=  c_ret_ok) return c_ret_nk;
	{
		tmr = tick_get();
		while (1)
		{
			if (tick_cmp(tmr, 60) == c_ret_ok) break;
			if (comm_read(&retLen, &app) == c_ret_ok)
			{
				if (__strstr(app.comm.rbuf, reinterpret_cast<const u8*>("dbgrx:ERROR")) != null)
					return c_ret_nk;

				if (retLen < 37) return c_ret_xk;

				for (x = 0; x < retLen - 36; x++)
				{
					if ((app.comm.rbuf[x + 0] == 0xaa)
						&& (app.comm.rbuf[x + 1] == 0xaa)
						&& (app.comm.rbuf[x + 2] == 0x07)
						&& (app.comm.rbuf[x + 3] == 32))
					{
						u8 i;
						sum = (u8)(0xaa + 0xaa + 0x07 + 32);
						for (i = 0; i < 32; i++)
							sum += app.comm.rbuf[x + 4 + i];
						if (sum == app.comm.rbuf[x + 4 + i])
						{
							app.comm.rbuf[x + 4 + i] = 0;
							memcpy(flash_id, &app.comm.rbuf[x + 4], 32*sizeof(u8));
							return c_ret_ok;
						}
						return c_ret_nk;
					}
				}
				return c_ret_xk;
			}
		}

	}
	return c_ret_tmo;


}

int com_controler::upload_data_to_flash(unsigned char* data, int data_lenght) {
	
	int ret = c_ret_nk;
	int retry = 0;
	comm_rx_clr(&app);
	if (comm_send(reinterpret_cast<u8*>("WRITE START"), sizeof("WRITE START") - 1, &app)!=c_ret_ok) return c_ret_nk;
	{
		tmr = tick_get();
		while (1)
		{
			if (tick_cmp(tmr, 11000) == c_ret_ok) break;
			if (comm_read(&retLen, &app) == c_ret_ok)
			{
				for (x = 0; x < retLen; x++)
				{
					if (app.comm.rbuf[x] == c_xmodem_nxk) {
						app.xmodem_upload_req = 0;
						app.xmodem_upload_run = 0;
						app.xmodem_download_req = 1;
						app.xmodem_download_addr = data;
						app.xmodem_download_len = data_lenght;
						return xmodem_download(&app);
					}
				}
				return c_ret_xk;
			}
		}
	}
	return c_ret_nk;
}

int com_controler::upload_data_to_flash_test(unsigned char* data, int data_lenght) {

	int ret = c_ret_nk;
	int retry = 0;
	comm_rx_clr(&app);

	if (comm_send(reinterpret_cast<u8*>("HimaxBoot"), sizeof("HimaxBoot") - 1, &app) != c_ret_ok) return c_ret_nk;
	tmr = tick_get();
	while (1)
	{
		if (tick_cmp(tmr, 11000) == c_ret_ok) break;
		if (comm_read(&retLen, &app) == c_ret_ok) {
			if (__strstr(app.comm.rbuf, reinterpret_cast<const u8*>("HimaxBoot OK")) != null) {
				if (comm_send(reinterpret_cast<u8*>("WRITE PARAM START"), sizeof("WRITE PARAM START") - 1, &app) != c_ret_ok) return c_ret_nk;
				tmr = tick_get();
				while (1)
				{
					if (tick_cmp(tmr, 11000) == c_ret_ok) return c_ret_nk;
					if (comm_read(&retLen, &app) == c_ret_ok)
					{
						for (x = 0; x < retLen; x++)
						{
							if (app.comm.rbuf[x] == c_xmodem_nxk) {
								app.xmodem_upload_req = 0;
								app.xmodem_upload_run = 0;
								app.xmodem_download_req = 1;
								app.xmodem_download_addr = data;
								app.xmodem_download_len = data_lenght;
								return xmodem_download(&app);
							}
						}
					}
				}
			}
		}

	}
	return c_ret_nk;
}

int com_controler::upload_data_to_flash_type(unsigned char* data, int data_lenght, int type) {

	int ret = c_ret_nk;
	comm_rx_clr(&app);
	
	char order[10];

	if (type == TYPE_D) {
		order[0] = 'D';
	}
	else if (type == TYPE_P) {
		order[0] = 'P';
	}
	else if (type == TYPE_T) {
		order[0] = 'T';
	}
	else return -1;

	if (comm_send(reinterpret_cast<u8*>(order), 1, &app) != c_ret_ok) return c_ret_nk;
	{
		tmr = tick_get();
		while (1)
		{
			if (tick_cmp(tmr, 11000) == c_ret_ok) break;
			if (__strstr(app.comm.rbuf, reinterpret_cast<const u8*>("Select")) != null) {
				tmr = tick_get();
				while (1) {
					if (tick_cmp(tmr, 11000) == c_ret_ok) break;
					if (comm_read(&retLen, &app) == c_ret_ok)
					{
						for (x = 0; x < retLen; x++)
						{
							if (app.comm.rbuf[x] == c_xmodem_nak) {
								app.xmodem_upload_req = 0;
								app.xmodem_upload_run = 0;
								app.xmodem_download_req = 1;
								app.xmodem_download_addr = data;
								app.xmodem_download_len = data_lenght;
								return xmodem_download(&app);
							}
						}
						return c_ret_xk;
					}
				}
			}
		}
	}
	return c_ret_nk;
}

int com_controler::download_data_from_flash(unsigned char* data, int data_lenght) {
	int ret = c_ret_nk;
	int retry = 0;
	comm_rx_clr(&app);
	if (c_ret_ok != comm_send(reinterpret_cast<u8*>("READ START"), sizeof("READ START") - 1, &app))
		return c_ret_nk;
	Sleep(100);
	 {
		tmr = tick_get();
		while (1)
		{
			if (tick_cmp(tmr, 11000) == c_ret_ok) break;
			if (comm_read(&retLen, &app) == c_ret_ok)
			{
				if (__strstr(app.comm.rbuf, reinterpret_cast<const u8*>("READ START OK")) != null) {
					app.xmodem_download_req = 0;
					app.xmodem_download_run = 0;
					app.xmodem_upload_req = 1;
					app.xmodem_upload_addr = data;
					app.xmodem_upload_len = data_lenght;
					return xmodem_upload(&app);
				}
				else if (__strstr(app.comm.rbuf, reinterpret_cast<const u8*>("dbgrx:ERROR")) != null)
					return c_ret_nk;
			}
		}
	}
	return c_ret_nk;
}



int com_controler::sendDataFix(std::string data, std::string want)
{
	comm_rx_clr(&app);
	__memcpy(buf, reinterpret_cast<const u8*>(data.c_str()), data.size());
	if (comm_send(buf, data.size(), &app) != c_ret_ok) return c_ret_nk;
	{
		if (want == "")
			return c_ret_ok;
		int retry = 0;
		while (1)
		{
			if (comm_read(&retLen, &app) == c_ret_ok)
			{
				std::string retdata(reinterpret_cast<char*>(&app.comm.rbuf), retLen);
				if (retdata.find(want) != std::string::npos) return c_ret_ok;
				else retry+=1;
			}
			if (retry == 5)
				return c_ret_nk;
			Sleep(1000);
		}

	}
	return c_ret_tmo;
}

int com_controler::close_comm() {
	if (app.comm.open == 1)
	{
		if (CloseHandle(app.comm.handle) != 0)
			app.comm.open = 0;
	}
	return c_ret_ok;
}

int com_controler::write_comm(std::string com,std::string &info)
{
	comm_rx_clr(&app);
	__memcpy(buf, reinterpret_cast<const u8*>(com.c_str()), com.size());
	if (comm_send(buf, com.size(), &app) != c_ret_ok) return c_ret_nk;
	{
		Sleep(100);
		tmr = tick_get();
		while (1)
		{
			if (tick_cmp(tmr, 60) == c_ret_ok) break;
			if (comm_read(&retLen, &app) == c_ret_ok)
			{
				if (retLen == 0) return c_ret_nk;
				info = std::string(reinterpret_cast<char*>(app.comm.rbuf), retLen);
				return c_ret_ok;
			}
		}
	}
	return c_ret_tmo;
}

int com_controler::readUtilWant(std::string want, int waitTime)
{
	comm_rx_clr(&app);
	tmr = tick_get();
	while (1)
	{
		if (waitTime!=0 && (tick_cmp(tmr, waitTime) == c_ret_ok)) break;
		if (comm_read(&retLen, &app) == c_ret_ok)
		{
			//printf("%s\n", app.comm.rbuf);
			if (retLen == 0) return c_ret_nk;
			if (__strstr(app.comm.rbuf, reinterpret_cast<const u8*>(want.c_str())) != null)
				return c_ret_ok;
		}
	}
	return c_ret_nk;
}

int com_controler::use_comm_handle(HANDLE handle)
{
	comm_init(&app);
	app.comm.open = 1;
	app.comm.handle = handle;

	return 0;
}

int com_controler::get_comm_handle(HANDLE & handle)
{
	if (app.comm.open == 0)
		return c_ret_nk;
	else {
		handle = app.comm.handle;
		return c_ret_ok;
	}
	return 0;
}
