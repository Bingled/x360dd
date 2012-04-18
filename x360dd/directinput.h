#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

void FreeDirectInput();
bool PollKeyboard( char * buffer);
HRESULT InitDirectInput( HWND hDlg );
