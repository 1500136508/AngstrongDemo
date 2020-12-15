// The next 3 includes are needed for serial port enumeration
#include <objbase.h>
#include <initguid.h>
#include <Setupapi.h>
#include <atlconv.h>
#include "EnumSerial.h"
#pragma comment(lib, "setupapi.lib")
// The following define is from ntddser.h in the DDK. It is also
// needed for serial port enumeration.
#ifndef GUID_CLASS_COMPORT
DEFINE_GUID(GUID_CLASS_COMPORT, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, \
			0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);
#endif

//---------------------------------------------------------------
// Helpers for enumerating the available serial ports.
// These throw a CString on failure, describing the nature of
// the error that occurred.

void EnumPortsWdm(std::vector<SSerInfo> &asi);
void EnumPortsWNt4(std::vector<SSerInfo> &asi);
void EnumPortsW9x(std::vector<SSerInfo> &asi);
void SearchPnpKeyW9x(HKEY hkPnp, BOOL bUsbDevice,
	std::vector<SSerInfo> &asi);

std::string WCHAR2String(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen <= 0)
		return std::string("");

	char* pszDst = new char[nLen];
	if (NULL == pszDst)
		return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;

	std::string strTmp(pszDst);
	delete[] pszDst;

	return strTmp;
}

//char* GetAnsiString(const CString &s)
//{
//	int nSize = 2 * s.GetLength();
//	char *pAnsiString = new char[nSize + 1];
//	wcstombs(pAnsiString, s, nSize + 1);
//	return pAnsiString;
//}
//---------------------------------------------------------------
// Routine for enumerating the available serial ports.
// Throws a CString on failure, describing the error that
// occurred. If bIgnoreBusyPorts is TRUE, ports that can't
// be opened for read/write access are not included.

void EnumSerialPorts(std::vector<SSerInfo> &asi, bool bIgnoreBusyPorts)
{
	// Clear the output array
	asi.clear();

	// Use different techniques to enumerate the available serial
	// ports, depending on the OS we're using
	OSVERSIONINFO vi;
	vi.dwOSVersionInfoSize = sizeof(vi);
	if (!::GetVersionEx(&vi)) 
	{
		std::string str("");
		throw str;
	}
	// Handle windows 9x and NT4 specially
	if (vi.dwMajorVersion < 5) {
		if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT)
			EnumPortsWNt4(asi);
		else
			EnumPortsW9x(asi);
	}
	else {
		// Win2k and later support a standard API for
		// enumerating hardware devices.
		EnumPortsWdm(asi);
	}

	//for (size_t sz = 0; sz < asi.size(); ++sz)
	//{
	//	SSerInfo rsi = asi.at(sz);    
	//	if (bIgnoreBusyPorts) {
	//		// Only display ports that can be opened for read/write
	//		HANDLE hCom = CreateFile((LPCWSTR)rsi.strDevPath.c_str(),
	//			GENERIC_READ | GENERIC_WRITE,
	//			0,    /* comm devices must be opened w/exclusive-access */
	//			NULL, /* no security attrs */
	//			OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
	//			0,    /* not overlapped I/O */
	//			NULL  /* hTemplate must be NULL for comm devices */
	//			);
	//		if (hCom == INVALID_HANDLE_VALUE) {
	//			// It can't be opened; remove it.
	//			asi.erase(sz);
	//			ii--;               
	//			continue;
	//		}
	//		else {
	//			// It can be opened! Close it and add it to the list
	//			::CloseHandle(hCom);
	//		}
	//	}

	//	// Come up with a name for the device.
	//	// If there is no friendly name, use the port name.
	//	if (rsi.strFriendlyName.empty())
	//		rsi.strFriendlyName = rsi.strPortName;

	//	// If there is no description, try to make one up from
	//	// the friendly name.
	//	if (rsi.strPortDesc.empty()) {
	//		// If the port name is of the form "ACME Port (COM3)"
	//		// then strip off the " (COM3)"
	//		rsi.strPortDesc = rsi.strFriendlyName;
	//		int startdex = rsi.strPortDesc.find(" (");
	//		int enddex = rsi.strPortDesc.find(")");
	//		if (startdex > 0 && enddex == (sizeof(rsi.strPortDesc) - 1))
	//		{
	//			for (size_t sz = startdex; startdex <= enddex; ++startdex)
	//			{
	//				rsi.strPortDesc += rsi.strPortDesc.at(sz);
	//			}
	//		}
	//	}
	//}
}

// Helpers for EnumSerialPorts

void EnumPortsWdm(std::vector<SSerInfo> &asi)
{
	std::string strErr;
	// Create a device information set that will be the container for 
	// the device interfaces.
	GUID *guidDev = (GUID*) &GUID_CLASS_COMPORT;

	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
	SP_DEVICE_INTERFACE_DETAIL_DATA *pDetData = NULL;

	try {
		hDevInfo = SetupDiGetClassDevs( guidDev,
			NULL,
			NULL,
			DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
			);

		if(hDevInfo == INVALID_HANDLE_VALUE) 
		{
			throw strErr;
		}

		// Enumerate the serial ports
		BOOL bOk = TRUE;
		SP_DEVICE_INTERFACE_DATA ifcData;
		DWORD dwDetDataSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 256;
		pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*) new char[dwDetDataSize];
		// This is required, according to the documentation. Yes,
		// it's weird.
		ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		pDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		for (DWORD ii=0; bOk; ii++) {
			bOk = SetupDiEnumDeviceInterfaces(hDevInfo,
				NULL, guidDev, ii, &ifcData);
			if (bOk) {
				// Got a device. Get the details.
				SP_DEVINFO_DATA devdata = {sizeof(SP_DEVINFO_DATA)};
				bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo,
					&ifcData, pDetData, dwDetDataSize, NULL, &devdata);
				if (bOk) {
					std::string strDevPath(WCHAR2String(pDetData->DevicePath));
					// Got a path to the device. Try to get some more info.
					TCHAR fname[256];
					TCHAR desc[256];
					BOOL bSuccess = SetupDiGetDeviceRegistryProperty(
						hDevInfo, &devdata, SPDRP_FRIENDLYNAME, NULL,
						(PBYTE)fname, sizeof(fname), NULL);
					bSuccess = bSuccess && SetupDiGetDeviceRegistryProperty(
						hDevInfo, &devdata, SPDRP_DEVICEDESC, NULL,
						(PBYTE)desc, sizeof(desc), NULL);
					BOOL bUsbDevice = FALSE;
					TCHAR locinfo[256];
					if (SetupDiGetDeviceRegistryProperty(
						hDevInfo, &devdata, SPDRP_LOCATION_INFORMATION, NULL,
						(PBYTE)locinfo, sizeof(locinfo), NULL))
					{
						// Just check the first three characters to determine
						// if the port is connected to the USB bus. This isn't
						// an infallible method; it would be better to use the
						// BUS GUID. Currently, Windows doesn't let you query
						// that though (SPDRP_BUSTYPEGUID seems to exist in
						// documentation only).
						//bUsbDevice = (strncmp(locinfo, "USB", 3)==0);
					}
					if (bSuccess) {
						// Add an entry to the array
						USES_CONVERSION; //这两句也要加入2
						char *char_fname = W2A(fname);
						char *char_desc = W2A(desc);
						SSerInfo si;
						si.strDevPath = strDevPath;
						si.strFriendlyName = char_fname;
						si.strPortDesc = char_desc;
						si.bUsbDevice = bUsbDevice;
						asi.push_back(si);
					}

				}
				else {
	
					throw strErr;
				}
			}
			else {
				DWORD err = GetLastError();
				if (err != ERROR_NO_MORE_ITEMS) {
					//strErr.Format("SetupDiEnumDeviceInterfaces failed. (err=%lx)", err);
					throw strErr;
				}
			}
		}
	}
	catch (std::string strCatchErr) {
		strErr = strCatchErr;
	}

	if (pDetData != NULL)
		delete [] (char*)pDetData;
	if (hDevInfo != INVALID_HANDLE_VALUE)
		SetupDiDestroyDeviceInfoList(hDevInfo);

	if (!strErr.empty())
		throw strErr;
}

void EnumPortsWNt4(std::vector<SSerInfo> &asi)
{
	// NT4's driver model is totally different, and not that
	// many people use NT4 anymore. Just try all the COM ports
	// between 1 and 16
	SSerInfo si;
	for (int ii=1; ii<=16; ii++) {
		std::string strPort;
		strPort="COM%d"+std::to_string(ii);
		si.strDevPath = "\\\\.\\" + strPort;
		si.strPortName = strPort;
		//asi.Add(si);
	}
}

void EnumPortsW9x(std::vector<SSerInfo> &asi)
{
	// Look at all keys in HKLM\Enum, searching for subkeys named
	// *PNP0500 and *PNP0501. Within these subkeys, search for
	// sub-subkeys containing value entries with the name "PORTNAME"
	// Search all subkeys of HKLM\Enum\USBPORTS for PORTNAME entries.

	// First, open HKLM\Enum
	HKEY hkEnum = NULL;
	HKEY hkSubEnum = NULL;
	HKEY hkSubSubEnum = NULL;

	try {
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, (LPCWSTR)"Enum", 0, KEY_READ,
			&hkEnum) != ERROR_SUCCESS)
			throw std::string("Could not read from HKLM\\Enum");

		// Enumerate the subkeys of HKLM\Enum
		char acSubEnum[128];
		DWORD dwSubEnumIndex = 0;
		DWORD dwSize = sizeof(acSubEnum);
		while (RegEnumKeyEx(hkEnum, dwSubEnumIndex++, (LPWSTR)acSubEnum, &dwSize,
			NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			HKEY hkSubEnum = NULL;
			if (RegOpenKeyEx(hkEnum, (LPWSTR)acSubEnum, 0, KEY_READ,
				&hkSubEnum) != ERROR_SUCCESS)
				throw std::string ("Could not read from HKLM\\Enum\\") + acSubEnum;

			// Enumerate the subkeys of HKLM\Enum\*\, looking for keys
			// named *PNP0500 and *PNP0501 (or anything in USBPORTS)
			BOOL bUsbDevice = (strcmp(acSubEnum,"USBPORTS")==0);
			char acSubSubEnum[128];
			dwSize = sizeof(acSubSubEnum);  // set the buffer size
			DWORD dwSubSubEnumIndex = 0;
			while (RegEnumKeyEx(hkSubEnum, dwSubSubEnumIndex++, (LPWSTR)acSubSubEnum,
				&dwSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
			{
				BOOL bMatch = (strcmp(acSubSubEnum,"*PNP0500")==0 ||
					strcmp(acSubSubEnum,"*PNP0501")==0 ||
					bUsbDevice);
				if (bMatch) {
					HKEY hkSubSubEnum = NULL;
					if (RegOpenKeyEx(hkSubEnum, (LPWSTR)acSubSubEnum, 0, KEY_READ,
						&hkSubSubEnum) != ERROR_SUCCESS)
						throw std::string("Could not read from HKLM\\Enum\\") + 
						acSubEnum + "\\" + acSubSubEnum;
					SearchPnpKeyW9x(hkSubSubEnum, bUsbDevice, asi);
					RegCloseKey(hkSubSubEnum);
					hkSubSubEnum = NULL;
				}

				dwSize = sizeof(acSubSubEnum);  // restore the buffer size
			}

			RegCloseKey(hkSubEnum);
			hkSubEnum = NULL;
			dwSize = sizeof(acSubEnum); // restore the buffer size
		}
	}
	catch (std::string strError) {
		if (hkEnum != NULL)
			RegCloseKey(hkEnum);
		if (hkSubEnum != NULL)
			RegCloseKey(hkSubEnum);
		if (hkSubSubEnum != NULL)
			RegCloseKey(hkSubSubEnum);
		throw strError;
	}

	RegCloseKey(hkEnum);
}

void SearchPnpKeyW9x(HKEY hkPnp, BOOL bUsbDevice,
	std::vector<SSerInfo> &asi)
{
	// Enumerate the subkeys of the given PNP key, looking for values with
	// the name "PORTNAME"
	// First, open HKLM\Enum
	HKEY hkSubPnp = NULL;

	try {
		// Enumerate the subkeys of HKLM\Enum\*\PNP050[01]
		char acSubPnp[128];
		DWORD dwSubPnpIndex = 0;
		DWORD dwSize = sizeof(acSubPnp);
		while (RegEnumKeyEx(hkPnp, dwSubPnpIndex++, (LPWSTR)acSubPnp, &dwSize,
			NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			HKEY hkSubPnp = NULL;
			if (RegOpenKeyEx(hkPnp, (LPWSTR)acSubPnp, 0, KEY_READ,
				&hkSubPnp) != ERROR_SUCCESS)
				throw std::string ("Could not read from HKLM\\Enum\\...\\")
				+ acSubPnp;

			// Look for the PORTNAME value
			char acValue[128];
			dwSize = sizeof(acValue);
			if (RegQueryValueEx(hkSubPnp, (LPCWSTR)"PORTNAME", NULL, NULL, (BYTE*)acValue,
				&dwSize) == ERROR_SUCCESS)
			{
				std::string strPortName(acValue);

				// Got the portname value. Look for a friendly name.
				std::string strFriendlyName;
				dwSize = sizeof(acValue);
				if (RegQueryValueEx(hkSubPnp, (LPCWSTR)"FRIENDLYNAME", NULL, NULL, (BYTE*)acValue,
					&dwSize) == ERROR_SUCCESS)
					strFriendlyName = acValue;

				// Prepare an entry for the output array.
				SSerInfo si;
				si.strDevPath = "\\\\.\\" + strPortName;
				si.strPortName = strPortName;
				si.strFriendlyName = strFriendlyName;
				si.bUsbDevice = bUsbDevice;

				// Overwrite duplicates.
				BOOL bDup = FALSE;
				for (int ii=0; ii<asi.size() && !bDup; ii++)
				{
					if (asi[ii].strPortName == strPortName) {
						bDup = TRUE;
						asi[ii] = si;
					}
				}
				if (!bDup) {
					// Add an entry to the array
					asi.push_back(si);
				}
			}

			RegCloseKey(hkSubPnp);
			hkSubPnp = NULL;
			dwSize = sizeof(acSubPnp);  // restore the buffer size
		}
	}
	catch (std::string strError) {
		if (hkSubPnp != NULL)
			RegCloseKey(hkSubPnp);
		throw strError;
	}
}
