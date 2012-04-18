extern char strMySystemFile[MAX_PATH];

int WriteToFile(const char *, const char *, char *);
int ReadFromFile(const char *, const char *, char *);
int ReadFromFile(const char *, const char *, char *, char *);
char * const DXErrStr(HRESULT dierr);
HWND GetWindowHandle(DWORD);
int WriteLog(char *,...);
LONG clamp(LONG val, LONG min, LONG max);