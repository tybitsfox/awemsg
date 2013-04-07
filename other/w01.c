#include "/workarea/cprogram/include/clsscr.h"
#include <curl/curl.h>
#include <curl/easy.h>
#ifndef BYTE
#define BYTE		unsigned char
#endif
#define bufsize		65536
//分析、显示数据的数量，如需更多显示可更改此处，并添加weather.rc中查询关键字和位置。
#define dnum		3
/*本程序将集成所有conky气象数据的获取操作，不带参数的调用将用于获取网页数据
 * 资料并保存为临时文件。/tmp/wthxx0316.dat
 * 带一个参数的调用将分别析取不同的显示数据并显示。目前为析取3个数据
 * 如需添加，可在weather.rc中添加相应的搜索关键字及查询位置，并且增大
 * dnum的大小。
 * 默认的资源文件为~/.conky/weather.rc
 * 为节约conky资源，还是使用临时文件将已经获取的数据保存其中，该文件默认为：/tmp/wthdata.dat
 */
/*
 * 2009年6月6日修改版，使用中文显示天气预报。
 * http://203.81.29.121/tq1.php?f=o&city=%cc%A9%B0%B2
 */
FILE *fp;
size_t write_data(void *p,size_t size,size_t number,void *stream);//curl要求的回调函数，用于保存为文件
int geturl();//获取气象web网页资料
int analy();//分析，获取数据
int fnd(BYTE* pd,BYTE* ps,int len,int dlen,int fd);//由analy调用，实现分析、保存数据
//{{{ int main(int argc,char** argv)
int main(int argc,char** argv)
{
	int i,j,k;
	pid_t pid;
	char ch[512];
	if(argc==1)
	{//获取网页并分析获得需要的数据，保存至临时文件wthdata.dat
normal_2:		
	/*	k=1;j=0;
		while(k!=0)
		{
			if(j>3)
			{return 0;}
			k=geturl();
			sleep(10);
			j++>3;
		}*/
		if(geturl()!=0)
		{
			//printf("geturl error\n");所有的显示在函数中实现
			return 0;
		}
		pid=fork();
		if(pid==0)
		{
			system("iconv -f GB2312 -t UTF-8 /tmp/wthxx0316.da > /tmp/wthxx0316.dat");
			return 0;
		}
		if(pid>0)
		{
			sleep(3);
			if(analy()!=0)
			{
				//printf("analysis error\n");在函数中实现
				return 0;
			}
		}
		goto normal_1;
	}
	else
	{
		if(argc==2)
		{//带有一个参数的调用，将实现不同数据的显示。
			i=atoi(argv[1]);
			if(i<1 || i>dnum)//保证参数的设置与资源文件中一致
			{
				goto perr_1;
			}
			FILE *fl;char *p1;
			fl=fopen("/tmp/wthdata.dat","r");
			if(fl==NULL)
			{
				if(i==1)
				{goto normal_2;}
				printf("open file error\n");
				goto normal_1;
			}
			for(j=0;j<i;j++)
			{//通过循环获得不同行的数据。
				memset(ch,0,sizeof(ch));
				p1=fgets(ch,sizeof(ch),fl);
				if(p1==NULL)
				{
					printf("get data error\n");
					fclose(fl);
					goto normal_1;
				}
			}
			printf("%s",ch);
			fclose(fl);
			goto normal_1;
		}
		else
		{
			goto perr_1;
		}
	}
perr_1:
	printf("get and save date usage:\n %s\ndisplay usage: %s num\n",argv[0],argv[0]);
normal_1:
	return 0;
}//}}}
//与服务器通讯，获取web数据，并保存于/tmp/wthxx0316.dat中
//感谢楼兰关于curl的提示！楼牛牛~~~~~~~~~~~~~~
//{{{ int geturl()
int geturl()
{
	char ch[512],uname[512];
	CURL *curl;
	int i,k;
	char *p;
	FILE *f;
	memset(ch,0,sizeof(ch));
	memset(uname,0,sizeof(uname));
	f=fopen("/root/.conky/weather.rc","r");//打开资源文件，读取url
	if(f==NULL)
	{
		printf("open rcfile error\n");
		return 1;
	}
	while(fgets(ch,sizeof(ch),f)!=NULL)
	{
		if(ch[0]=='u' && ch[1]==':')
		{
			memcpy(uname,&ch[2],strlen(ch)-3);//-3 del \n
			break;
		}
	}
	fclose(f);
	if(strlen(uname)<=0)
	{
		printf("get url error\n");
		return 1;
	}
	fp=fopen("/tmp/wthxx0316.da","w"); //modify here
	if(fp==NULL)
	{
		printf("open file error 2012-2-13\n");
		return 1;
	}
	curl_global_init(CURL_GLOBAL_ALL);
	curl=curl_easy_init();
	if(curl==NULL)
	{//http://tieba.baidu.com/f?kw=linux&fn=50
		printf("init curl error\n");
		fclose(fp);
		return 1;
	}
	k=0;
	k=curl_easy_setopt(curl,CURLOPT_URL,uname);
	if(k!=0)
	{
		printf("erty01\n");
		goto  aa_001;
	}
	k=curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_data);
	if(k!=0)
	{
		printf("erty02\n");
		goto aa_001;
	}
	k=curl_easy_perform(curl);
	if(k!=0)
	{
		printf("erty03\n");
		goto aa_001;
	}
aa_001:	
	curl_easy_cleanup(curl);
	if(fp!=NULL)
	{
		fclose(fp);
	}
	return k;
}//}}}
//分析网页资料文件：/tmp/wthxx0316.dat，获取结果值并保存为文件：/tmp/wthdata.dat
//{{{int analy()
int analy()
{
	char c[dnum][512];
	int  n[dnum];
	char ch[512],*p;
	int f,sf,i,j,k,l;
	fp=fopen("/root/.conky/weather.rc","r");
	if(fp==NULL)
	{
		printf("open rc error\n");
		return 1;
	}
	for(i=0;i<dnum;i++)
	{
		n[i]=0;
		memset(c[i],0,sizeof(c[i]));
	}
	memset(ch,0,sizeof(ch));
	while(fgets(ch,sizeof(ch),fp)!=NULL)
	{
		if(ch[0]=='n' && ch[1]=='1')
		{
			n[0]=atoi(&ch[2]);
			goto rl_1;
		}
		if(ch[0]=='n' && ch[1]=='2')
		{
			n[1]=atoi(&ch[2]);
			goto rl_1;
		}
		if(ch[0]=='n' && ch[1]=='3')
		{
			n[2]=atoi(&ch[2]);
			goto rl_1;
		}
		if(ch[0]=='n' && ch[1]=='4')
		{
			if(dnum<4)
				goto rl_1;
			n[3]=aoti(&ch[2]);
			goto rl_1;
		}
		if(ch[0]=='n' && ch[1]=='5')
		{
			if(dnum<5)
				goto rl_1;
			n[4]=atoi(&ch[2]);
			goto rl_1;
		}
		if(ch[0]=='W' && ch[1]==':')
		{
			memcpy(c[0],&ch[2],strlen(ch)-3);
			goto rl_1;
		}
		if(ch[0]=='T' && ch[1]==':')
		{
			memcpy(c[1],&ch[2],strlen(ch)-3);
			goto rl_1;
		}
		if(ch[0]=='t' && ch[1]==':')
		{
			memcpy(c[2],&ch[2],strlen(ch)-3);
			goto rl_1;
		}//other paras are not used in this proc
		//if you want use para 4 or 5,add them at here	
rl_1:
		memset(ch,0,sizeof(ch));	
	}
	fclose(fp);
	for(i=0;i<dnum;i++)
	{
		if(n[i]<=0)
		{
			printf("error to get index\n");
			return 1;
		}
		if(strlen(c[i])<=0)
		{
			printf("error to get search condition string\n");
			return 1;
		}
	}
	p=malloc(bufsize);
	if(p==NULL)
	{
		printf("malloc error\n");
		return 1;
	}
	f=open("/tmp/wthxx0316.dat",O_RDONLY);
	if(f<0)
	{
		printf("open file1 error\n");
		free(p);
		return 1;
	}
	sf=open("/tmp/wthdata.dat",O_RDWR|O_CREAT|O_TRUNC,0644);
	if(sf<0)
	{
		printf("open file2 error\n");
		close(f);
		free(p);
		return 1;
	}
	l=0;
	for(j=0;j<dnum;j++)
	{
		k=strlen(c[j]);
		lseek(f,0,SEEK_SET);
		do{
			memset(p,0,bufsize);
			i=read(f,p,bufsize);
			if(fnd((BYTE*)p,(BYTE*)c[j],i,n[j],sf)==0)
			{
				l++;
				break;
			}
			lseek(f,-k,SEEK_CUR);//回移k的指针位移，保证分段读取数据造成查询的遗漏
		}while(i==bufsize);
	}
	close(f);
	close(sf);
	free(p);
	if(l!=dnum)
	{//查找的结果数量与预期不符
		printf("somewhere error\n");
		return 1;
	}
	return 0;
}//}}}
//具体的比较分析
//{{{  int fnd(BYTE* pd,BYTE* ps,int len,int dlen,int fd)
int fnd(BYTE* pd,BYTE* ps,int len,int dlen,int fd)
{
	char c[100],c1[100];
	int i,j,k,l;
	l=strlen(ps);
	for(i=0;i<(len-1);i++)
	{
		k=0;
		for(j=0;j<l;j++)
		{
			if(((BYTE)pd[i+j]) != ((BYTE)ps[j]))
			{
				k=1;
				break;
			}
		}
		if(k==0)
		{// printf("ok~\n");
			memset(c,0,sizeof(c));
			k=0;
			for(j=(i+l);j<len;j++)
			{
				if(pd[j]=='>')
				{
					k++;
					if(k==dlen)
					{
						j++;i=0;
						while(j<len)
						{
							c[i++]=pd[j++];
							if(pd[j]=='<')
							{
								k=strlen(c);
								if(c[i] != '\n')
									c[i]='\n';
								write(fd,c,i+1);
								return 0;
							}
						}
					}
				}
			}
		}
	}
	return 1;
}//}}}
//{{{  curl需要的回调函数
size_t write_data(void *p,size_t size,size_t number,void *stream)
{
	int written = fwrite(p, size, number, (FILE *)fp);
	return written;
}//}}}


