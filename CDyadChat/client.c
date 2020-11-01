#include <pthread.h>
#include <dyad.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_MAX 256

pthread_t thread;

void *threadFunction(void *arg) {
	char line[STRING_MAX];
	for(;;) {
		fgets(line,STRING_MAX,stdin);
		dyad_writef((dyad_Stream*)arg,"%s",line);
	}
}

static void onConnect(dyad_Event *e) {
	pthread_create(&thread,NULL,threadFunction,e->stream);
}

static void onError(dyad_Event *e) {
  printf("error: %s\n", e->msg);
}

static void onLine(dyad_Event *e) {
  printf("%s\n", e->data);
}

static void onClose(dyad_Event *e) {
  printf("error: closed connetion\n");
}

int main(int argc,char **argv) {
  dyad_Stream *s;

	if(argc!=3) {
		fprintf(stderr,"Syntax: %s HOST PORT\n",argv[0]);
		return -1;
	}

  dyad_init();

  s = dyad_newStream();
  dyad_addListener(s, DYAD_EVENT_CONNECT, onConnect, NULL);
  dyad_addListener(s, DYAD_EVENT_ERROR,   onError,   NULL);
  dyad_addListener(s, DYAD_EVENT_LINE,    onLine,    NULL);
  dyad_addListener(s, DYAD_EVENT_CLOSE,   onClose,   NULL);
  dyad_connect(s, argv[1], atoi(argv[2]));

  while (dyad_getStreamCount() > 0) {
    dyad_update();
  }

  dyad_shutdown();

  return 0;
}
