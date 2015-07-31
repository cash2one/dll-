#ifndef LOLINFO_H
#define LOLINFO_H

const enum ReportType {
	ReportLoLHelper_Arrive = 0x1,			//LoLHelper客户端生效
	ReportAirLibErrNum_HookFail = 0x2,		//AirLib.dll Hook失败
	ReportAirLibAdobeAirVersionNotSupported = 0x4,	//AdobeAirVersion不支持
	ReportInjectAdobeAirDLLFail = 0x8,		//注入AirLib.dll失败
	ReportGetReportHeadFail = 0x10,			//获取loginData头部失败
};

#endif