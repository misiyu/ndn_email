#ifndef _DATA_BASE_H_
#define _DATA_BASE_H_

#include <iostream>
#include <sstream>
#include <fstream>
#include "com_tool.h"
using std::string ;

class Data_base
{
public:
	Data_base();
	~Data_base();
	int save(char * buff , int buff_sz , int file_id);
	int append(char * buff , int buff_sz , int file_id);
	int append_table(char * buff , int buff_sz , int file_id);
	int read_file(char * buff  , int file_id , int & file_sz);
	int get_file_sz(int file_id);
	int get_e_list(string e_addr , string & e_list);
	int get_cur_id(){
		string e_file = this->m_e_dir+"email-"+std::to_string(this->cur_id) ;
		remove(e_file.data());
		return this->cur_id ;
	}
	void cur_id_p1(){
		this->cur_id ++ ;
		FILE * file = fopen(this->m_e_id_file.data(),"w");
		fprintf(file , "%d" , this->cur_id);
		fclose(file);
	}

private:
	string m_e_dir ;
	string m_table ;
	// user,fileid,r/u
	string m_e_id_file ;
	int cur_id ;
};

#endif 
