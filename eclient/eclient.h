#ifndef _ECLIENT_H_
#define _ECLIENT_H_

#include <unistd.h>
#include <ndn-cxx/face.hpp>
#include <errno.h>
#include <iostream>
#include <cstdlib>
#include <jsoncpp/json/json.h>

#include "data_pro.h"
#include "../common/recv_data.h"

using std::cout ;
using std::endl ;
using std::string ;
//using ndn::Interest ;
//using ndn::Data ;
//using ndn::lp::Nack ;
using namespace ndn ;

class EClient{

	private :
		ndn::Face m_face ;
		string m_ndn_addr ;
		string m_e_addr ;
		string serverPrefix ; 

		int slice_sum ;
		//Data_pro * m_data_pro;
		void onData(const Interest& interest , const Data& data);
		void onNack(const Interest& interest, const lp::Nack& nack);
		void onTimeout(const Interest& interest);
	public :
		
		EClient(string e_addr) ;
		int send_email(const string e_addr , int e_addr_type
				, const string message , int message_type ) ;
		int recv_email(const string e_addr , string key , int file_id) ;
		int get_email_list(const string e_addr , string key) ;
		int get_file_sz(const string e_addr , string key , int file_id) ;
		string ea2na(const string e_addr) ;
		void m_expressInterest(string interest_name ,
				void(EClient::*onData)(const Interest&, const Data& ));

};

#endif 
