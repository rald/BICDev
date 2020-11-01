#include <windows.h>
#include <pthread.h>
#include <dyad.h>

#include <stdarg.h>
#include <string.h>
#include <tchar.h>

#include "resource.h"

HWND hEdit1,hEdit2;
HFONT hFont;

WNDPROC oldEdit2Proc;

dyad_Stream *s,*stream=NULL;

pthread_t thread;

void AppendText(HWND hwnd,char *fmt,...) {
  va_list args;
  
  int len=0;
  char *buf1=NULL;
  int outLength=0;
  TCHAR *buf2;
  
  va_start(args,fmt);
  len=_vscprintf(fmt,args);
  va_end(args);

  buf1=calloc(len,sizeof(char));
  
  va_start(args,fmt);
  vsprintf(buf1,fmt,args);
  va_end(args);

	outLength=GetWindowTextLength(hwnd)+lstrlen(buf1)+1;
	buf2=(TCHAR*)GlobalAlloc(GPTR,outLength*sizeof(TCHAR));
	GetWindowText(hwnd,buf2,outLength);
	_tcscat_s(buf2,outLength,buf1);
	SetWindowText(hwnd,buf2);
	GlobalFree(buf2);
	free(buf1);
	buf1=NULL;

  SendMessage(hEdit1,EM_SETSEL,0,-1);
  SendMessage(hEdit1,EM_SETSEL,-1,-1);
  SendMessage(hEdit1,EM_SCROLLCARET,0,0);
}

static void onConnect(dyad_Event *e) {
  stream=e->stream;
  AppendText(hEdit1,"connected\r\n");
}

static void onError(dyad_Event *e) {
  AppendText(hEdit1,"error: %s\r\n", e->msg);
}

static void onLine(dyad_Event *e) {
  AppendText(hEdit1,"%s\r\n", e->data);
}

static void onClose(dyad_Event *e) {
  AppendText(hEdit1,"error: closed connetion\r\n");
}

void *threadFunction(void *arg) {
  dyad_init();

  s = dyad_newStream();
  dyad_addListener(s, DYAD_EVENT_CONNECT, onConnect, NULL);
  dyad_addListener(s, DYAD_EVENT_ERROR,   onError,   NULL);
  dyad_addListener(s, DYAD_EVENT_LINE,    onLine,    NULL);
  dyad_addListener(s, DYAD_EVENT_CLOSE,   onClose,   NULL);
  dyad_connect(s,"127.0.0.1",5254);

  while (dyad_getStreamCount() > 0) {
    dyad_update();
  }

  dyad_shutdown();
  
  return NULL;
}


long FAR PASCAL subEdit2Proc(		
		HWND hwnd, 
		WORD msg, 
		WORD wParam, 
		LONG lParam) {

	switch(msg) {
    case WM_GETDLGCODE:
      return (DLGC_WANTALLKEYS | CallWindowProc(oldEdit2Proc,hwnd,msg,wParam,lParam));
    case WM_CHAR:
      if(wParam==VK_RETURN || wParam==VK_TAB) {
        return 0;
      } else {
        return CallWindowProc(oldEdit2Proc,hwnd,msg,wParam,lParam);  
      }
		case WM_KEYDOWN:
      if(wParam==VK_RETURN) {
        int outLength=GetWindowTextLength(hEdit2)+1;
				TCHAR *buf=(TCHAR*) GlobalAlloc(GPTR,outLength*sizeof(TCHAR));
				GetWindowText(hEdit2,buf,outLength);
				AppendText(hEdit1,"%s\r\n",buf);
        SetWindowText(hEdit2,"");
        if(stream) dyad_writef(stream,"%s\r\n",buf);
        GlobalFree(buf);
        return 0;
      } else {
        return CallWindowProc(oldEdit2Proc,hwnd,msg,wParam,lParam);  
			}
    default:
      return CallWindowProc(oldEdit2Proc,hwnd,msg,wParam,lParam);  
	}
  return 0;
}

BOOL CALLBACK DlgProc(
		HWND hwnd, 
		UINT msg, 
		WPARAM wParam, 
		LPARAM lParam) {
	
	switch(msg) {
		case WM_INITDIALOG:

			hEdit1=GetDlgItem(hwnd,IDC_EDIT1);
			hEdit2=GetDlgItem(hwnd,IDC_EDIT2);
			
			hFont=CreateFont(20,0,0,0,0,FALSE,0,0,0,0,0,0,0,"Courier");

			SendMessage(hEdit1,WM_SETFONT,(WPARAM)hFont,TRUE);
			SendMessage(hEdit2,WM_SETFONT,(WPARAM)hFont,TRUE);

			oldEdit2Proc=(WNDPROC)SetWindowLongPtr(hEdit2,GWLP_WNDPROC,(LONG_PTR)subEdit2Proc);
			
			pthread_create(&thread,NULL,threadFunction,NULL);
			
			break;
		case WM_SIZE: {
			UINT w=LOWORD(lParam);
			UINT h=HIWORD(lParam);
			SetWindowPos(hEdit1,NULL,2,2,w-4,h-39,SWP_NOZORDER);
			SetWindowPos(hEdit2,NULL,2,h-32,w-4,30,SWP_NOZORDER);
			break; }
		case WM_CLOSE:
			EndDialog(hwnd,0);
			break;
		default:
			return FALSE;
	}
	
	return TRUE;
}

int WINAPI WinMain(
		HINSTANCE hinst,
		HINSTANCE hprevinst,
		LPSTR cmdLine,
		int cmdShow) {
	
	return DialogBox(hinst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);	
	
}
