#ifndef LOLINFO_H
#define LOLINFO_H

const enum ReportType {
	ReportLoLHelper_Arrive = 0x1,			//LoLHelper�ͻ�����Ч
	ReportAirLibErrNum_HookFail = 0x2,		//AirLib.dll Hookʧ��
	ReportAirLibAdobeAirVersionNotSupported = 0x4,	//AdobeAirVersion��֧��
	ReportInjectAdobeAirDLLFail = 0x8,		//ע��AirLib.dllʧ��
	ReportGetReportHeadFail = 0x10,			//��ȡloginDataͷ��ʧ��
};

#endif