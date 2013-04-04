#include"clsscr.h"
//定义监视的项目数量
#define	 jc			4
#define  chlen		1024
#define  mem_len	256
#define  weather	"/root/.conky/weather"
#define  wfile		"/tmp/wthdata.dat"
#define	 awesome	"awesome-client"
//电池电量获取所需
#define sfile   "/sys/class/power_supply/BAT0/uevent"
#define power_base  "POWER_SUPPLY_ENERGY_FULL="
#define power_now   "POWER_SUPPLY_ENERGY_NOW="
//CPU
#define cpu_file	"/proc/stat"
//memory
#define mem_file	"/proc/meminfo"
//定义显示信息及色彩
#define	 col_cyan	"#00ffff"
#define  col_red	"#ff0000"
#define  col_green  "#00ff00"
#define  col_blue	"#0000ff"
#define  col_yellow	"#ffff00"
#define	 col_dpink	"#ff00ff"
//#define	 out_msg	"<span color='%s'>CPU频率:%s |电量:%s |泰安%s %s </span>"
#define	 out_msg	"conkytext.text = \"<span color='%s'>| CPU:%s|内存:%s %s| 电量:%s| 泰安 %s %s |</span>\""
struct T_J
{
	int n;          //number of task
	char c;//[1024];	//command--no use
};
struct T_J tj[4]; 
char fmt[chlen];
FILE	*pipef;
int	 cpufd,memfd;
void *cpu_area,*mem_area;
//static int t_j[jc];
//msg[5]=cpu,msg[3,4]=mem,msg[2]=bat,msg[1,0]=weather
char   msg[6][100];
int	   cpu_v[4];
int chg_daemon();
void get_config();
void format_msg(int i);
int disp_msg();
void get_batt();//battery get
void get_cpu();//cpu status
void get_mem();//mem status
void get_net();//net up/down
int  crt_mmap();//create cpu&mem mmap
void clear_own();//close all handle.

