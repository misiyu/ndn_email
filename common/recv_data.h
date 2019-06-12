#ifndef _RECV_DATA_H_
#define _RECV_DATA_H_

#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ndn-cxx/face.hpp>
#include <errno.h>
#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <jsoncpp/json/json.h>
#include <fstream>

#include "data_base.h"
#include "com_tool.h"

#define P_DATA_SZ 8002

using std::string ;
using std::cout ;
using std::endl ;
using namespace ndn ;

class Recv_data
{
public:
	Recv_data();
	Recv_data(string interst_name , int segment_num , Data_base &db);
	~Recv_data();
	pthread_t get_ptid(){
		return this->m_ptid ;
	}
	bool is_finish(){ return this->finish; }
	void start();
	void onData(const Interest& interest , const Data& data);
	void onNack(const Interest& interest, const lp::Nack& nack);
	void onTimeout(const Interest& interest) ;
	static void * run(void *param){
		Recv_data * _this = (Recv_data*)param ;

		std::stringstream ss ;
		ss << getSystemTime() << "/"<< _this->m_get_segment_count ;
		string interest_name = _this->m_interest_name+"/"+ss.str() ;

		std::cout << "expressInterest : " << interest_name << endl ;
		Interest interest(Name(interest_name.data()));
		interest.setInterestLifetime(2_s);
		_this->m_face.expressInterest(interest , 
				bind(&Recv_data::onData,_this,_1,_2),
				bind(&Recv_data::onNack,_this,_1,_2),
				bind(&Recv_data::onTimeout,_this,_1));
		_this->m_face.processEvents(time::milliseconds::zero(),true);
		_this->finish = true ;
	}

private:
	int m_segment_num ;
	int m_get_segment_count ;
	string m_interest_name ;
	Face m_face ;
	pthread_t m_ptid ;
	int m_file_id ;
	Data_base &m_db ;

	bool finish ;

};

#endif 
