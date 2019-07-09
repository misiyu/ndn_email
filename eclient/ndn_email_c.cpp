#include <iostream>
#include "eclient.h"

using namespace std;


void parse_args(int argc , char ** args , string &cmd , string &e_addr , 
		string &msg , int &file_id){
	if(argc <=2 ){
		cout << "Usage : ./ndn_email_c -(s,l,r) <e_addr> <msg> <file_id>" << endl;
		exit(1);
	}
	cmd = args[1] ;
	e_addr = args[2] ;
	msg = args[3];
	if(argc > 4) file_id = atoi(args[4]);
}

int main(int argc , char ** args)
{
	EClient eclient("misiyu@b.com");
	string cmd ;
	string e_addr ;
	string msg ;
	int file_id ;

	parse_args(argc, args , cmd , e_addr , msg , file_id) ;

	if(cmd == "-s"){
		int addr_type = 0 ;
		if(e_addr.find('@',0) != string::npos) addr_type = 1;
		eclient.send_email(e_addr , addr_type , msg , 0);
	}else if(cmd == "-l"){
		eclient.get_email_list(e_addr , "123");
	}else if(cmd == "-r"){
		eclient.recv_email(e_addr , "123" , file_id);
	}

	//if(n == 0)
		//eclient.send_email("111@qq.com" , 1 , "123@b.com,misiyu@b.com,hello\n" , 0);
	//else if (n ==1)
		//eclient.get_email_list("misiyu@b.com","123");
	
	//else
		//eclient.recv_email("misiyu@b.com","123",0) ;


	return 0;
}
