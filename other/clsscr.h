#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/select.h>   //use for select function
#include<sys/time.h>     //use for select function
#include<sys/socket.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<fcntl.h>
#include<netinet/tcp.h>
#include<errno.h>
#include<signal.h>
#include<ftw.h>
#include<dirent.h>
//#include<mysql/mysql.h>
#include<stdarg.h>
//IPC USED
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/shm.h>
#include<syslog.h>
//ptrace use
#include<sys/ptrace.h>
#include<sys/user.h>
#include<sys/utsname.h>
#ifndef CLS_OWNE
#define CLS_OWNE		"\033[2J\033[1;1H"
#define CLS()			printf(CLS_OWNE)
#endif

#define max(a,b)         ((a)>=(b)?(a):(b))
#define min(a,b)	 ((a)>=(b)?(b):(a))

struct tsg_1
{
	char ch[20];
};
