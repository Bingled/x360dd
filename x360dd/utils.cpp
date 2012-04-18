#include "stdafx.h"
#include "extern.h"
#include "utils.h"

char strMySystemFile[MAX_PATH];	
int writelog = 0;
char *logfilename;
int logready = 0;

char * const DXErrStr(HRESULT dierr) {
	if (dierr == DIERR_ACQUIRED) return "DIERR_ACQUIRED";
	if (dierr == DI_BUFFEROVERFLOW) return "DI_BUFFEROVERFLOW";
	if (dierr == DI_DOWNLOADSKIPPED) return "DI_DOWNLOADSKIPPED";
	if (dierr == DI_EFFECTRESTARTED) return "DI_EFFECTRESTARTED";
	if (dierr == DI_NOEFFECT) return "DI_NOEFFECT";
	if (dierr == DI_NOTATTACHED) return "DI_NOTATTACHED";
	if (dierr == DI_OK) return "DI_OK";
	if (dierr == DI_POLLEDDEVICE) return "DI_POLLEDDEVICE";
	if (dierr == DI_PROPNOEFFECT) return "DI_PROPNOEFFECT";
	if (dierr == DI_SETTINGSNOTSAVED) return "DI_SETTINGSNOTSAVED";
	if (dierr == DI_TRUNCATED) return "DI_TRUNCATED";
	if (dierr == DI_TRUNCATEDANDRESTARTED) return "DI_TRUNCATEDANDRESTARTED";
	if (dierr == DI_WRITEPROTECT) return "DI_WRITEPROTECT";
	if (dierr == DIERR_ACQUIRED) return "DIERR_ACQUIRED";
	if (dierr == DIERR_ALREADYINITIALIZED) return "DIERR_ALREADYINITIALIZED";
	if (dierr == DIERR_BADDRIVERVER) return "DIERR_BADDRIVERVER";
	if (dierr == DIERR_BETADIRECTINPUTVERSION) return "DIERR_BETADIRECTINPUTVERSION";
	if (dierr == DIERR_DEVICEFULL) return "DIERR_DEVICEFULL";
	if (dierr == DIERR_DEVICENOTREG) return "DIERR_DEVICENOTREG";
	if (dierr == DIERR_EFFECTPLAYING) return "DIERR_EFFECTPLAYING";
	if (dierr == DIERR_GENERIC) return "DIERR_GENERIC";
	if (dierr == DIERR_HANDLEEXISTS) return "DIERR_HANDLEEXISTS";
	if (dierr == DIERR_HASEFFECTS) return "DIERR_HASEFFECTS";
	if (dierr == DIERR_INCOMPLETEEFFECT) return "DIERR_INCOMPLETEEFFECT";
	if (dierr == DIERR_INPUTLOST) return "DIERR_INPUTLOST";
	if (dierr == DIERR_INVALIDPARAM) return "DIERR_INVALIDPARAM";
	if (dierr == DIERR_MAPFILEFAIL) return "DIERR_MAPFILEFAIL";
	if (dierr == DIERR_MOREDATA) return "DIERR_MOREDATA";
	if (dierr == DIERR_NOAGGREGATION) return "DIERR_NOAGGREGATION";
	if (dierr == DIERR_NOINTERFACE) return "DIERR_NOINTERFACE";
	if (dierr == DIERR_NOTACQUIRED) return "DIERR_NOTACQUIRED";
	if (dierr == DIERR_NOTBUFFERED) return "DIERR_NOTBUFFERED";
	if (dierr == DIERR_NOTDOWNLOADED) return "DIERR_NOTDOWNLOADED";
	if (dierr == DIERR_NOTEXCLUSIVEACQUIRED) return "DIERR_NOTEXCLUSIVEACQUIRED";
	if (dierr == DIERR_NOTFOUND) return "DIERR_NOTFOUND";
	if (dierr == DIERR_NOTINITIALIZED) return "DIERR_NOTINITIALIZED";
	if (dierr == DIERR_OBJECTNOTFOUND) return "DIERR_OBJECTNOTFOUND";
	if (dierr == DIERR_OLDDIRECTINPUTVERSION) return "DIERR_OLDDIRECTINPUTVERSION";
	if (dierr == DIERR_OTHERAPPHASPRIO) return "DIERR_OTHERAPPHASPRIO";
	if (dierr == DIERR_OUTOFMEMORY) return "DIERR_OUTOFMEMORY";
	if (dierr == DIERR_READONLY) return "DIERR_READONLY";
	if (dierr == DIERR_REPORTFULL) return "DIERR_REPORTFULL";
	if (dierr == DIERR_UNPLUGGED) return "DIERR_UNPLUGGED";
	if (dierr == DIERR_UNSUPPORTED) return "DIERR_UNSUPPORTED";
	if (dierr == E_HANDLE) return "E_HANDLE";
	if (dierr == E_PENDING) return "E_PENDING";
	if (dierr == E_POINTER) return "E_POINTER";
	return "Unknown error code";
}


void GetTime(int &year, int &month, int &day, int &hour, int &min, int &sec ){
	{
		time_t rawtime;
		struct tm timeinfo;
		time ( &rawtime );
		localtime_s(&timeinfo,&rawtime);  

		year = timeinfo.tm_year+1900;
		month = timeinfo.tm_mon+1;
		day = timeinfo.tm_mday;
		hour = timeinfo.tm_hour;
		min = timeinfo.tm_min;
		sec = timeinfo.tm_sec;

	}
}

int WriteToFile(const char * strFileSection, const char * strKey, char * strValue)
{
	//if ((strlen(strKey) > MAX_PATH) | (strlen(strValue) > MAX_PATH))
	//	return -1;
	return WritePrivateProfileString(strFileSection, strKey, strValue, strMySystemFile);
}

int ReadFromFile(const char * strFileSection, const char * strKey, char * strOutput)
{
	return ReadFromFile(strFileSection, strKey, strOutput, NULL);
}

int ReadFromFile(const char * strFileSection, const char * strKey, char * strOutput, char * strDefault)
{
	return GetPrivateProfileString(strFileSection, strKey, strDefault, strOutput, MAX_PATH, strMySystemFile);
}

int WriteLog(char *str,...)
{
	if (writelog){
		int year = 0;
		int month = 0;
		int day = 0;
		int hour = 0;
		int min = 0;
		int sec = 0;

		GetTime(year, month, day, hour, min, sec );

		if(logready == 0){ // checking if file name for log is ready, if not create it
			logfilename = new char[MAX_PATH];
			sprintf_s(logfilename,MAX_PATH,"x360kb logs\\x360kb %d%02d%02d-%02d%02d%02d.log",year,month,day,hour,min,sec);
			logready = 1;
		}

		if(GetFileAttributes("x360kb logs") == INVALID_FILE_ATTRIBUTES) CreateDirectory("x360kb logs", NULL);
		FILE * fp;
		fopen_s(&fp,logfilename,"a");

		//fp is null, file is not open.
		if (fp==NULL)
			return -1;
		fprintf(fp,"%d/%02d/%02d %02d:%02d:%02d:: ",year,month,day,hour,min,sec);
		va_list arglist;
		va_start(arglist,str);
		vfprintf(fp,str,arglist);
		va_end(arglist);
		fprintf(fp," \n");
		fclose(fp);

		return 0;
	}
	return -1;
}


LONG clamp(LONG val, LONG min, LONG max) {
	if (val < min) return min;
	if (val > max) return max;
	return val;
}