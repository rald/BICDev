#include <dyad.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


#define CLIENT_MAX 10

dyad_Stream *clients[CLIENT_MAX];

static void broadcast(const char *fmt,...) {
	int i;
	va_list args;
	va_start(args,fmt);
  vprintf(fmt,args);
	va_end(args);
	for(i=0;i<CLIENT_MAX;i++) {
		if(clients[i]) {
			va_start(args,fmt);
		  dyad_vwritef(clients[i],fmt,args);
			va_end(args);
		}
	}
}

static void onError(dyad_Event *e) {
  printf("error: %s\n", e->msg);
}

static void onLine(dyad_Event *e) {
	broadcast("%d: %s\r\n",*(int*)e->udata,e->data);
}

static void onClose(dyad_Event *e) {
	broadcast("%d -> disconnected.\r\n",*(int*)e->udata);
	clients[*(int*)e->udata]=NULL;
	free(e->udata);
}

static void onAccept(dyad_Event *e) {
	int i,j;

	j=-1;
  for(i=0;i<CLIENT_MAX;i++) {
		if(clients[i]==NULL) {
			j=i;
			break;
		}
	}

	if(j!=-1) {
		int *k=malloc(sizeof(int));
		clients[j]=e->remote;
		*k=j;
	  dyad_addListener(e->remote, DYAD_EVENT_LINE,    onLine,    k);
	  dyad_addListener(e->remote, DYAD_EVENT_CLOSE,   onClose,   k);
	  dyad_addListener(e->remote, DYAD_EVENT_ERROR,   onError,   k);
		dyad_writef(e->remote,"you are number %d.\r\n",j);
		broadcast("%d -> joins.\r\n",j);
	} else {
		printf("error: server full.\n");
		dyad_writef(e->remote,"error: server full.\r\n");
		dyad_end(e->remote);
	}

}

int main(int argc,char **argv) {

	int i;
  dyad_Stream *s;

	if(argc!=2) {
		fprintf(stderr,"Syntax: %s PORT\n",argv[0]);
		return -1;
	}

	for(i=0;i<CLIENT_MAX;i++) clients[i]=NULL;

  dyad_init();

  s = dyad_newStream();
  dyad_addListener(s, DYAD_EVENT_ACCEPT,  onAccept,  NULL);
  dyad_addListener(s, DYAD_EVENT_ERROR,   onError,   NULL);
  dyad_listen(s, atoi(argv[1]));

  while (dyad_getStreamCount() > 0) {
    dyad_update();
  }

  dyad_shutdown();

  return 0;
}
