#ifndef _COM_TOOL_H_
#define _COM_TOOL_H_

#include <iostream>
#include <sys/timeb.h>
#include <sys/stat.h>

using std::string ;

//static unsigned int P_DATA_SZ = 8002
#define P_DATA_SZ 8002

static long long getSystemTime(){
	timeb t;
	ftime(&t);
	return t.time * 1000 + t.millitm;
}

static string get_raw_string(string str){
	int tmp = str.find_first_of('%');
	if(tmp == std::string::npos) return str ;
	return str.substr(0,tmp)+"@"+str.substr(tmp+3,str.length()-tmp-3);
}


static unsigned long get_file_size(const char *path , int & segment_num)
{
	unsigned long filesize = -1;	
	struct stat statbuff;
	if(stat(path, &statbuff) < 0){
		return filesize;
	}else{
		filesize = statbuff.st_size;
	}
	segment_num = filesize/(P_DATA_SZ-2) ;
	if(filesize % (P_DATA_SZ-2) != 0) segment_num ++ ;
	return filesize;
}

#endif 
