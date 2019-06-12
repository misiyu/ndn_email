#ifndef _ESERVER_H_
#define _ESERVER_H_

#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <ndn-cxx/face.hpp>
#include <errno.h>
#include <exception>

#include "recv_data.h"
#include "../common/data_pro.h"

using namespace ndn ;
using std::cout ;
using std::endl ;
using std::string ;

class EServer
{
public:
	EServer();
	~EServer();

private:
	Face m_face ;
	KeyChain m_keyChain ;
	string m_listen_addr;
	Data_base m_db ;

	Recv_data *rd_pool[20];
	Data_pro *dp_pool[20];


	void onInterest(const InterestFilter& filter, const Interest& interest) ;
	void onRegisterFailed(const Name& prefix, const std::string& reason);
	int start_a_rdt(string cli_ndn_addr , int slice_sum);
	int start_a_dpt(const char * send_data , int data_len , string ndn_addr);
};

#endif 
