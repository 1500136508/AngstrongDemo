//////////////////////////////////////////////////////////////////////
// Video Capture using DirectShow
// Author: Shiqi Yu (shiqi.yu@gmail.com)
// Thanks to:
//		HardyAI@OpenCV China
//		flymanbox@OpenCV China (for his contribution to function CameraName, and frame width/height setting)
// Last modification: April 9, 2009
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// 使用说明：
//   1. 将CameraDS.h CameraDS.cpp以及目录DirectShow复制到你的项目中
//   2. 菜单 Project->Settings->Settings for:(All configurations)->C/C++->Category(Preprocessor)->Additional include directories
//      设置为 DirectShow/Include
//   3. 菜单 Project->Settings->Settings for:(All configurations)->Link->Category(Input)->Additional library directories
//      设置为 DirectShow/Lib
//////////////////////////////////////////////////////////////////////

// CameraDS.cpp: implementation of the CCameraDS class.
//
//////////////////////////////////////////////////////////////////////

#include "CameraDS.h"
#include "imagereader.h"
#pragma comment(lib,"Strmiids.lib")
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define FORMAT_YUV2 844715353
#define FORMAT_MJPG 1196444237

#define MYFREEMEDIATYPE(mt)	{if ((mt).cbFormat != 0)		\
                    {CoTaskMemFree((PVOID)(mt).pbFormat);	\
                    (mt).cbFormat = 0;						\
                    (mt).pbFormat = NULL;					\
                }											\
                if ((mt).pUnk != NULL)						\
                {											\
                    (mt).pUnk->Release();					\
                    (mt).pUnk = NULL;						\
                }}
CCameraDS::CCameraDS()
{
    m_bConnected = m_bLock = m_bChanged = false;
    m_nWidth = m_nHeight = 0;
    m_nBufferSize = 0;
	camera_id_ = 0;

    m_pFrame = NULL;

    m_pNullFilter = NULL;
    m_pMediaEvent = NULL;
    m_pSampleGrabberFilter = NULL;
    m_pGraph = NULL;

	//COM Library Initialize
	if (FAILED(CoInitialize(NULL)))
	{
		QString err_msg = "CoInitialize Failed!\r\n";
	}


    m_nBuffer = new unsigned short[1280 * 800 * 3];
}

CCameraDS::~CCameraDS()
{
	if (imoniker_video_)
	{
		imoniker_video_->Release();
		imoniker_video_ = nullptr;
	}
	for (size_t i = 0; i < 10; ++i)
	{
		if (imoniker_container[i])
		{
			imoniker_container[i]->Release();
			imoniker_container[i] = nullptr;
		}
	}
    CloseCamera();
    CoUninitialize();

    delete[] m_nBuffer;
}

void CCameraDS::CloseCamera()
{
    if (m_bConnected)
    {
        m_pMediaControl->Stop();
    }

    m_pGraph = NULL;
    m_pDeviceFilter = NULL;
    m_pMediaControl = NULL;
    m_pSampleGrabberFilter = NULL;
    m_pSampleGrabber = NULL;
    m_pGrabberInput = NULL;
    m_pGrabberOutput = NULL;
    m_pCameraOutput = NULL;
    m_pMediaEvent = NULL;
    m_pNullFilter = NULL;
    m_pNullInputPin = NULL;

    if (m_pFrame)
    {
        cvReleaseImage(&m_pFrame);
    }

    m_bConnected = m_bLock = m_bChanged = false;
    m_nWidth = m_nHeight = 0;
    m_nBufferSize = 0;
}

bool CCameraDS::isOpened()
{
    return m_bConnected;
}

bool CCameraDS::Live()
{
	bool bReturn = false;
	do
	{
		if (!m_bConnected)
			break;
		if (!m_pMediaControl)
			break;
		if (!m_pMediaControl->Run())
			break;

		bReturn = true;
	} while (false);
	return bReturn;
}

bool CCameraDS::Pause()
{
	bool bReturn = false;
	do
	{
		if (!m_bConnected)
			break;
		if (!m_pMediaControl)
			break;
		if (!m_pMediaControl->Pause())
			break;

		bReturn = true;
	} while (false);
	return bReturn;
}

bool CCameraDS::Stop()
{
	bool bReturn = false;
	do
	{
		if (!m_bConnected)
			break;
		if (!m_pMediaControl)
			break;
		if (!m_pMediaControl->Stop())
			break;

		bReturn = true;
	} while (false);
	return bReturn;
}

bool CCameraDS::OpenCamera(int nCamID, int nWidth, int nHeight, bool isYUV2)
{
	try
	{
		HRESULT hr = S_OK;
		isFormatYUY2 = isYUV2;
		if (m_bConnected)
		{
			if (m_pMediaControl)
				m_pMediaControl->Stop();
		}
		
		CoInitialize(NULL);
		// Create the Filter Graph Manager.
		
		//hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&m_pGraph);
		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (LPVOID*) &m_pGraph);

		hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (LPVOID *)&m_pSampleGrabberFilter);

		hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
		hr = m_pGraph->QueryInterface(IID_IMediaEvent, (void **)&m_pMediaEvent);

		hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (LPVOID*)&m_pNullFilter);

		hr = m_pGraph->AddFilter(m_pNullFilter, L"NullRenderer");

		hr = m_pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&m_pSampleGrabber);

		AM_MEDIA_TYPE   mt;
		ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
		mt.majortype = MEDIATYPE_Video;
		if (isYUV2) mt.subtype = MEDIASUBTYPE_YUY2;
		else mt.subtype = MEDIASUBTYPE_MJPG;
		mt.formattype = FORMAT_VideoInfo;
		hr = m_pSampleGrabber->SetMediaType(&mt);
		MYFREEMEDIATYPE(mt);

		m_pGraph->AddFilter(m_pSampleGrabberFilter, L"Grabber");

		// Bind Device Filter.  We know the device because the id was passed in
		if (!BindFilter(nCamID, &m_pDeviceFilter))
			return false;

		m_pGraph->AddFilter(m_pDeviceFilter, NULL);
		//Create the capture graph builder
		CComPtr<IEnumPins> pEnum;
		m_pDeviceFilter->EnumPins(&pEnum);

		hr = pEnum->Reset();
		hr = pEnum->Next(1, &m_pCameraOutput, NULL);

		pEnum = NULL;
		m_pSampleGrabberFilter->EnumPins(&pEnum);
		pEnum->Reset();
		hr = pEnum->Next(1, &m_pGrabberInput, NULL);

		pEnum = NULL;
		m_pSampleGrabberFilter->EnumPins(&pEnum);
		pEnum->Reset();
		pEnum->Skip(1);
		hr = pEnum->Next(1, &m_pGrabberOutput, NULL);

		pEnum = NULL;
		m_pNullFilter->EnumPins(&pEnum);
		pEnum->Reset();
		hr = pEnum->Next(1, &m_pNullInputPin, NULL);

		//SetCrossBar();
		{
			//////////////////////////////////////////////////////////////////////////////
			// 加入由 lWidth和lHeight设置的摄像头的宽和高 的功能，默认320*240
			// by flymanbox @2009-01-24
			//////////////////////////////////////////////////////////////////////////////
			IAMStreamConfig *iconfig = NULL;
			hr = m_pCameraOutput->QueryInterface(IID_IAMStreamConfig, (void**)&iconfig);

			AM_MEDIA_TYPE *pmtConfig;

			int iCount = 0, iSize = 0;
			hr = iconfig->GetNumberOfCapabilities(&iCount, &iSize);

			if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS)) {
				for (int iFormat = 0; iFormat < iCount; iFormat++)
				{
					VIDEO_STREAM_CONFIG_CAPS scc;
					hr = iconfig->GetStreamCaps(iFormat, &pmtConfig, (BYTE *)&scc);

					if (SUCCEEDED(hr)) {
						if (isYUV2 && pmtConfig->majortype == MEDIATYPE_Video && pmtConfig->formattype == FORMAT_VideoInfo
							&& pmtConfig->subtype == MEDIASUBTYPE_YUY2)
						{
							VIDEOINFOHEADER *phead = (VIDEOINFOHEADER*)(pmtConfig->pbFormat);
							phead->bmiHeader.biWidth = nWidth;
							phead->bmiHeader.biHeight = nHeight;
							phead->bmiHeader.biCompression = MAKEFOURCC('Y', 'U', 'Y', '2');

							if ((hr = iconfig->SetFormat(pmtConfig)) != S_OK)
							{
								return false;
							}
							if (FAILED(hr))
							{
								MessageBox(NULL, TEXT("SetFormat Failed\n"), NULL, MB_OK);
							}
							break;
						}
						else if (!isYUV2 && pmtConfig->majortype == MEDIATYPE_Video && pmtConfig->formattype == FORMAT_VideoInfo
							&& pmtConfig->subtype == MEDIASUBTYPE_MJPG) {
							VIDEOINFOHEADER *phead = (VIDEOINFOHEADER*)(pmtConfig->pbFormat);
							phead->bmiHeader.biWidth = nWidth;
							phead->bmiHeader.biHeight = nHeight;
							//phead->bmiHeader.biCompression = MAKEFOURCC('M', 'J', 'P', 'G');

							if ((hr = iconfig->SetFormat(pmtConfig)) != S_OK)
							{
								return false;
							}
							if (FAILED(hr))
							{
								MessageBox(NULL, TEXT("SetFormat Failed\n"), NULL, MB_OK);
							}
							break;
						}
					}

				}
			}
			iconfig->Release();
			iconfig = NULL;
			MYFREEMEDIATYPE(*pmtConfig);
		}

		hr = m_pGraph->Connect(m_pCameraOutput, m_pGrabberInput);
		hr = m_pGraph->Connect(m_pGrabberOutput, m_pNullInputPin);

		if (FAILED(hr))
		{
			switch (hr)
			{
			case VFW_S_NOPREVIEWPIN:
				break;
			case E_FAIL:
				break;
			case E_INVALIDARG:
				break;
			case E_POINTER:
				break;
			}
		}

		m_pSampleGrabber->SetBufferSamples(TRUE);
		m_pSampleGrabber->SetOneShot(TRUE);

		hr = m_pSampleGrabber->GetConnectedMediaType(&mt);
		if (FAILED(hr))
		{
			return false;
		}

		VIDEOINFOHEADER *videoHeader;
		videoHeader = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);
		m_nWidth = videoHeader->bmiHeader.biWidth;
		m_nHeight = videoHeader->bmiHeader.biHeight;
		m_nFormatType = videoHeader->bmiHeader.biCompression;

		m_bConnected = true;

		pEnum = NULL;
		//hr = m_pDeviceFilter->QueryInterface(&m_pCameraControl);
		//hr = m_pDeviceFilter->QueryInterface(&m_pVideoProAmp);
		FindDevice();
		if (imoniker_container[nCamID])
		{
			imoniker_video_ = imoniker_container[nCamID];
		}
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool CCameraDS::OpenCamera(std::string pid, std::string vid, int nWidth, int nHeight, bool isYUV2)
{
    HRESULT hr = S_OK;
    isFormatYUY2 = isYUV2;
    CoInitialize(NULL);
    // Create the Filter Graph Manager.
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&m_pGraph);

    hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (LPVOID *)&m_pSampleGrabberFilter);

    hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
    hr = m_pGraph->QueryInterface(IID_IMediaEvent, (void **)&m_pMediaEvent);

    hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (LPVOID*)&m_pNullFilter);

    hr = m_pGraph->AddFilter(m_pNullFilter, L"NullRenderer");

    hr = m_pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&m_pSampleGrabber);

    AM_MEDIA_TYPE   mt;
    ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
    mt.majortype = MEDIATYPE_Video;
    if (isYUV2) mt.subtype = MEDIASUBTYPE_YUY2;
    else mt.subtype = MEDIASUBTYPE_MJPG;
    mt.formattype = FORMAT_VideoInfo;
    hr = m_pSampleGrabber->SetMediaType(&mt);
    MYFREEMEDIATYPE(mt);

    m_pGraph->AddFilter(m_pSampleGrabberFilter, L"Grabber");

    // Bind Device Filter.  We know the device because the id was passed in
    if (!BindFilter(&m_pDeviceFilter, pid, vid))
        return false;
    m_pGraph->AddFilter(m_pDeviceFilter, NULL);

    CComPtr<IEnumPins> pEnum;
    m_pDeviceFilter->EnumPins(&pEnum);

    hr = pEnum->Reset();
    hr = pEnum->Next(1, &m_pCameraOutput, NULL);

    pEnum = NULL;
    m_pSampleGrabberFilter->EnumPins(&pEnum);
    pEnum->Reset();
    hr = pEnum->Next(1, &m_pGrabberInput, NULL);

    pEnum = NULL;
    m_pSampleGrabberFilter->EnumPins(&pEnum);
    pEnum->Reset();
    pEnum->Skip(1);
    hr = pEnum->Next(1, &m_pGrabberOutput, NULL);

    pEnum = NULL;
    m_pNullFilter->EnumPins(&pEnum);
    pEnum->Reset();
    hr = pEnum->Next(1, &m_pNullInputPin, NULL);

    //SetCrossBar();
    {
        //////////////////////////////////////////////////////////////////////////////
        // 加入由 lWidth和lHeight设置的摄像头的宽和高 的功能，默认320*240
        // by flymanbox @2009-01-24
        //////////////////////////////////////////////////////////////////////////////
        IAMStreamConfig *iconfig = NULL;
        hr = m_pCameraOutput->QueryInterface(IID_IAMStreamConfig, (void**)&iconfig);

        AM_MEDIA_TYPE *pmtConfig;

        int iCount = 0, iSize = 0;
        hr = iconfig->GetNumberOfCapabilities(&iCount, &iSize);

        if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS)) {
            for (int iFormat = 0; iFormat < iCount; iFormat++)
            {
                VIDEO_STREAM_CONFIG_CAPS scc;
                hr = iconfig->GetStreamCaps(iFormat, &pmtConfig, (BYTE *)&scc);

                if (SUCCEEDED(hr)) {
                    if (isYUV2 && pmtConfig->majortype == MEDIATYPE_Video && pmtConfig->formattype == FORMAT_VideoInfo
                        && pmtConfig->subtype == MEDIASUBTYPE_YUY2)
                    {
                        VIDEOINFOHEADER *phead = (VIDEOINFOHEADER*)(pmtConfig->pbFormat);
                        phead->bmiHeader.biWidth = nWidth;
                        phead->bmiHeader.biHeight = nHeight;
                        phead->bmiHeader.biCompression = MAKEFOURCC('Y', 'U', 'Y', '2');

                        if ((hr = iconfig->SetFormat(pmtConfig)) != S_OK)
                        {
                            return false;
                        }
                        if (FAILED(hr))
                        {
                            MessageBox(NULL, TEXT("SetFormat Failed\n"), NULL, MB_OK);
                        }
                        break;
                    }
                    else if (!isYUV2 && pmtConfig->majortype == MEDIATYPE_Video && pmtConfig->formattype == FORMAT_VideoInfo
                        && pmtConfig->subtype == MEDIASUBTYPE_MJPG) {
                        VIDEOINFOHEADER *phead = (VIDEOINFOHEADER*)(pmtConfig->pbFormat);
                        phead->bmiHeader.biWidth = nWidth;
                        phead->bmiHeader.biHeight = nHeight;
                        //phead->bmiHeader.biCompression = MAKEFOURCC('M', 'J', 'P', 'G');

                        if ((hr = iconfig->SetFormat(pmtConfig)) != S_OK)
                        {
                            return false;
                        }
                        if (FAILED(hr))
                        {
                            MessageBox(NULL, TEXT("SetFormat Failed\n"), NULL, MB_OK);
                        }
                        break;
                    }
                }

            }
        }
        iconfig->Release();
        iconfig = NULL;
        MYFREEMEDIATYPE(*pmtConfig);
    }

    hr = m_pGraph->Connect(m_pCameraOutput, m_pGrabberInput);
    hr = m_pGraph->Connect(m_pGrabberOutput, m_pNullInputPin);

    if (FAILED(hr))
    {
        switch (hr)
        {
        case VFW_S_NOPREVIEWPIN:
            break;
        case E_FAIL:
            break;
        case E_INVALIDARG:
            break;
        case E_POINTER:
            break;
        }
    }

    m_pSampleGrabber->SetBufferSamples(TRUE);
    m_pSampleGrabber->SetOneShot(TRUE);

    hr = m_pSampleGrabber->GetConnectedMediaType(&mt);
    if (FAILED(hr))
    {
        return false;
    }

    VIDEOINFOHEADER *videoHeader;
    videoHeader = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);
    m_nWidth = videoHeader->bmiHeader.biWidth;
    m_nHeight = videoHeader->bmiHeader.biHeight;
    m_nFormatType = videoHeader->bmiHeader.biCompression;

    m_bConnected = true;

    pEnum = NULL;
    hr = m_pDeviceFilter->QueryInterface(&m_pCameraControl);
    hr = m_pDeviceFilter->QueryInterface(&m_pVideoProAmp);

    return true;
}

bool CCameraDS::FindDevice()
{
	bool bReturn = false;
	do 
	{
		UINT    uIndex = -1;
		HRESULT hr;
		is_camera_stiil_here_ = false;
		// enumerate all video capture devices
		ICreateDevEnum *pCreateDevEnum = 0;
		hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
			IID_ICreateDevEnum, (void**)&pCreateDevEnum);
		if (hr != NOERROR)
		{
			std::string msg = "Error Creating Device Enumerator";
			break;
		}

		IEnumMoniker *pEm = 0;
		hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
		if (hr != NOERROR)
		{
			std::string msg = "Sorry, you have no video capture hardware.\r\n\r\n";
			/*for (size_t i = 0; i < 10; ++i)
			{
				if (imoniker_container[i])
				{
					imoniker_container[i]->Release();
					imoniker_container[i] = nullptr;
				}
			}*/
			/*if (imoniker_video_)
			{
				imoniker_video_->Release();
				imoniker_video_ = nullptr;
			}*/
			break;
		}

		pEm->Reset();
		ULONG cFetched;
		IMoniker *pM;

		while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK)
		{
			imoniker_container[uIndex+1] = pM;
			IPropertyBag *pBag = 0;

			hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
			if (SUCCEEDED(hr))
			{
				VARIANT var;
				var.vt = VT_BSTR;
				hr = pBag->Read(L"FriendlyName", &var, NULL);
				if (hr == NOERROR)
				{
					if (imoniker_video_ != nullptr && (S_OK == imoniker_video_->IsEqual(pM)))
					{
						camera_id_ = uIndex;
						is_camera_stiil_here_ = true;
					}
					SysFreeString(var.bstrVal);
					pM->AddRef();
				}
				pBag->Release();
			}

			pM->Release();
			uIndex++;
		}
		pEm->Release();
		if (!is_camera_stiil_here_)
		{
			camera_id_ = 0;
		}

		bReturn = true;
	} while (false);
	return bReturn;
}

bool CCameraDS::CameraStillHere()
{
	bool breturn = FindDevice();
	return is_camera_stiil_here_;
}


bool CCameraDS::BindFilter(int nCamID, IBaseFilter **pFilter)
{
    if (nCamID < 0)
    {
        return false;
    }
    if (nCamID >= CameraCount())
        return false;
    // enumerate all video capture devices
    CComPtr<ICreateDevEnum> pCreateDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pCreateDevEnum);
    if (hr != NOERROR)
    {
        return false;
    }

    CComPtr<IEnumMoniker> pEm;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
    if (hr != NOERROR)
    {
        return false;
    }

    pEm->Reset();
    ULONG cFetched;
    IMoniker *pM;
    int index = 0;
    while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK, index <= nCamID)
    {
        IPropertyBag *pBag;
        hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
        if (SUCCEEDED(hr))
        {
            VARIANT var;
            var.vt = VT_BSTR;
            hr = pBag->Read(L"FriendlyName", &var, NULL);
            if (hr == NOERROR)
            {
                if (index == nCamID)
                {
                    pM->BindToObject(0, 0, IID_IBaseFilter, (void**)pFilter);
                }
                SysFreeString(var.bstrVal);
            }
            pBag->Release();
        }
        pM->Release();
        index++;
    }

    pCreateDevEnum = NULL;
    return true;
}

bool CCameraDS::BindFilter(IBaseFilter ** pFilter, std::string pid, std::string vid)
{

    int camNum = CameraCount();
    CComPtr<ICreateDevEnum> pCreateDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pCreateDevEnum);
    if (hr != NOERROR)
    {
        return false;
    }

    CComPtr<IEnumMoniker> pEm;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
    if (hr != NOERROR)
    {
        return false;
    }

    pEm->Reset();
    ULONG cFetched;
    IMoniker *pM;
    int index = 0;
    bool camFound = false;
    while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK, index < camNum)
    {
        IPropertyBag *pBag;
        hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
        if (SUCCEEDED(hr))
        {
            VARIANT var;
            var.vt = VT_BSTR;
            hr = pBag->Read(L"DevicePath", &var, NULL);
            if (hr == NOERROR)
            {
                DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, var.bstrVal, -1, NULL, 0, NULL, FALSE);
                char *psText = new char[dwNum];
                WideCharToMultiByte(CP_OEMCP, NULL, var.bstrVal, -1, psText, dwNum, NULL, FALSE);
                std::string devicePath = psText;
                delete[] psText;
                std::transform(devicePath.begin(), devicePath.end(), devicePath.begin(), ::toupper);
                if (devicePath.find(pid) != std::string::npos && devicePath.find(vid) != std::string::npos)
                {
                    pM->BindToObject(0, 0, IID_IBaseFilter, (void**)pFilter);
                    camFound = true;
                }
                SysFreeString(var.bstrVal);
            }
            pBag->Release();
        }
        pM->Release();
        if (camFound)
            break;
        index++;
    }
    pCreateDevEnum = NULL;
    if (index == camNum)
        return false;

    return true;
}

void CCameraDS::SetCallBack(imageReader * function)
{
	m_pSampleGrabber->SetCallback(function, 1);
}

//将输入crossbar变成PhysConn_Video_Composite
void CCameraDS::SetCrossBar()
{
    int i;
    IAMCrossbar *pXBar1 = NULL;
    ICaptureGraphBuilder2 *pBuilder = NULL;

    HRESULT hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void **)&pBuilder);

    if (SUCCEEDED(hr))
    {
        hr = pBuilder->SetFiltergraph(m_pGraph);
    }

    hr = pBuilder->FindInterface(&LOOK_UPSTREAM_ONLY, NULL, m_pDeviceFilter, IID_IAMCrossbar, (void**)&pXBar1);

    if (SUCCEEDED(hr))
    {
        long OutputPinCount, InputPinCount;
        long PinIndexRelated, PhysicalType;
        long inPort = 0, outPort = 0;

        pXBar1->get_PinCounts(&OutputPinCount, &InputPinCount);
        for (i = 0; i < InputPinCount; i++)
        {
            pXBar1->get_CrossbarPinInfo(TRUE, i, &PinIndexRelated, &PhysicalType);
            if (PhysConn_Video_Composite == PhysicalType)
            {
                inPort = i;
                break;
            }
        }
        for (i = 0; i < OutputPinCount; i++)
        {
            pXBar1->get_CrossbarPinInfo(FALSE, i, &PinIndexRelated, &PhysicalType);
            if (PhysConn_Video_VideoDecoder == PhysicalType)
            {
                outPort = i;
                break;
            }
        }

        if (S_OK == pXBar1->CanRoute(outPort, inPort))
        {
            pXBar1->Route(outPort, inPort);
        }
        pXBar1->Release();
    }
    pBuilder->Release();
}

bool CCameraDS::SetExposure(int iExposure)
{
    long min = 0, max = 0, def = 0, step = 0, text = 0;;
    HRESULT hr;

    if (m_pCameraControl != NULL)
    {
        hr = m_pCameraControl->GetRange(CameraControl_Exposure, &min, &max, &step, &def, &text);
        if (iExposure<min || iExposure>max) iExposure = def;

        if (S_OK == (hr = m_pCameraControl->Set(CameraControl_Exposure, iExposure, CameraControl_Flags_Manual)))
            return true;

    }
    return false;
}

bool CCameraDS::getID(int &id)
{
    long min = 0, max = 0, def = 0, step = 0, text = 0;;
    HRESULT hr;

    if (m_pCameraControl != NULL)
    {
        hr = m_pCameraControl->Get(CameraControl_Exposure, &min, &max);
        printf("%ld,%ld,%ld,%ld,%ld\n",min, max, step,def, text);


        return true;

    }
    return false;
}

bool CCameraDS::Set(int flag, int value)
{
    switch (flag)
    {
    case DS_CAP_PROP_BRIGHTNESS:
        if (S_OK == m_pVideoProAmp->Set(VideoProcAmp_Brightness, value, VideoProcAmp_Flags_Manual))
            return true;
        break;
    case DS_CAP_PROP_EXPOSURE:
        return SetExposure(value);
        break;
    case DS_CAP_PROP_CONTRAST:
        if (S_OK == m_pVideoProAmp->Set(VideoProcAmp_Contrast, value, VideoProcAmp_Flags_Manual))
            return true;
        break;
    case DS_CAP_PROP_HUE:
        if (S_OK == m_pVideoProAmp->Set(VideoProcAmp_Hue, value, VideoProcAmp_Flags_Manual))
            return true;
        break;
    case DS_CAP_PROP_SATURATION:
        if (S_OK == m_pVideoProAmp->Set(VideoProcAmp_Saturation, value, VideoProcAmp_Flags_Manual))
            return true;
        break;
    case DS_CAP_PROP_SHARPNESS:
        if (S_OK == m_pVideoProAmp->Set(VideoProcAmp_Sharpness, value, VideoProcAmp_Flags_Manual))
            return true;
        break;
    case DS_CAP_PROP_GAMMA:
        if (S_OK == m_pVideoProAmp->Set(VideoProcAmp_Gamma, value, VideoProcAmp_Flags_Manual))
            return true;
        break;
    case DS_CAP_PROP_GAIN:
        if (S_OK == m_pVideoProAmp->Set(VideoProcAmp_Gain, value, VideoProcAmp_Flags_Manual))
            return true;
        break;

    default:
        break;
    }
    return false;
}

/*
The returned image can not be released.
*/
IplImage* CCameraDS::QueryFrame()
{
    long evCode, size = 0;

    m_pMediaControl->Run();
    m_pMediaEvent->WaitForCompletion(INFINITE, &evCode);

    m_pSampleGrabber->GetCurrentBuffer(&size, NULL);

    printf("Image Size :%d\n", size);
    //if the buffer size changed
    if (size != m_nBufferSize)
    {
        if (m_pFrame)
        {
            cvReleaseImage(&m_pFrame);
        }

        m_nBufferSize = size;
        m_pFrame = cvCreateImage(cvSize(m_nWidth, m_nHeight), IPL_DEPTH_8U, 3);
    }

    m_pSampleGrabber->GetCurrentBuffer(&m_nBufferSize, (long*)m_pFrame->imageData);
    cvFlip(m_pFrame);

    return m_pFrame;
}

bool CCameraDS::read(cv::Mat & outputArray)
{
    long evCode, size = 0;

    m_pMediaControl->Run();
    m_pMediaEvent->WaitForCompletion(INFINITE, &evCode);
    m_pSampleGrabber->GetCurrentBuffer(&size, NULL);

    if (size != 0) {
        if (isFormatYUY2) {
            m_nBufferSize = size;
            m_pSampleGrabber->GetCurrentBuffer(&size, (long*)m_nBuffer);
            outputArray = cv::Mat(cv::Size(m_nWidth, m_nHeight), CV_16SC1, m_nBuffer);
        }
        else {
            m_nBufferSize = size;
            m_pSampleGrabber->GetCurrentBuffer(&size, (long*)m_nBuffer);
            cv::Mat src(m_nHeight, m_nWidth, CV_8UC3, (unsigned char*)m_nBuffer);
            outputArray = cv::imdecode(src, CV_LOAD_IMAGE_COLOR);
        }
        return true;
    }
    else {
        return false;
    }

}

bool CCameraDS::readRawData(unsigned char* data)
{
	long evCode, size = 0;

	m_pMediaControl->Run();
	//m_pMediaEvent->WaitForCompletion(INFINITE, &evCode);
	m_pMediaEvent->WaitForCompletion(2000, &evCode);//yxl
	m_pSampleGrabber->GetCurrentBuffer(&size, NULL);

	if (size != 0)
	{
		m_nBufferSize = size;
		m_pSampleGrabber->GetCurrentBuffer(&size, (long*)data);
		return true;
	}
	else
	{
		return false;
	}
}


int CCameraDS::CameraCount()
{
    int count = 0;
    CoInitialize(NULL);

    // enumerate all video capture devices
    CComPtr<ICreateDevEnum> pCreateDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pCreateDevEnum);

    CComPtr<IEnumMoniker> pEm;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
    if (hr != NOERROR)
    {
        return count;
    }

    pEm->Reset();
    ULONG cFetched;
    IMoniker *pM;
    while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK)
    {
        count++;
    }

    pCreateDevEnum = NULL;
    pEm = NULL;
    return count;
}

int CCameraDS::CameraName(int nCamID, char* sName, int nBufferSize)
{
    int count = 0;
    //CoInitialize(NULL);

    // enumerate all video capture devices
    CComPtr<ICreateDevEnum> pCreateDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pCreateDevEnum);

    CComPtr<IEnumMoniker> pEm;
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
    if (hr != NOERROR) return 0;

    pEm->Reset();
    ULONG cFetched;
    IMoniker *pM;
    while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK)
    {
        if (count == nCamID)
        {
            IPropertyBag *pBag = 0;
            hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
            if (SUCCEEDED(hr))
            {
                VARIANT var;
                var.vt = VT_BSTR;
                hr = pBag->Read(L"FriendlyName", &var, NULL); //还有其他属性,像描述信息等等...
                if (hr == NOERROR)
                {
                    //获取设备名称
                    WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, sName, nBufferSize, "", NULL);

                    SysFreeString(var.bstrVal);
                }
                pBag->Release();
            }
            pM->Release();

            break;
        }
        count++;
    }

    pCreateDevEnum = NULL;
    pEm = NULL;

    return 1;
}

bool CCameraDS::SetAutoExposure()
{
    long min = 0, max = 0, def = 0, step = 0, text = 0;;
    HRESULT hr;

    if (m_pCameraControl != NULL)
    {
        //if (S_OK == (hr = m_pCameraControl->GetRange(CameraControl_Exposure, &min, &max, &step, &def, &text)))
        {
            if (S_OK == (hr = m_pCameraControl->Set(CameraControl_Exposure, 0, CameraControl_Flags_Auto)))
                return true;
        }
    }
    return false;
}
