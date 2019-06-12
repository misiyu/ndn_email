#ifndef _DATA_PRO_H_
#define _DATA_PRO_H_
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <ndn-cxx/face.hpp>
#include <errno.h>
#include <exception>


#define P_DATA_SZ 8002

using namespace ndn ;
using std::string ;

class Data_pro{

	private :
		pthread_t m_tid ;		
		char *m_data_buff ;
		int m_data_len ;
		int slice_sum ;
		int s_data_c ;
		bool finish ;

		Face m_face ;
		string m_listen_addr ;
		KeyChain m_keyChain ;
	public:
		Data_pro( const char * send_data , int data_len , string listen_addr);
		~Data_pro();
		void onInterest(const InterestFilter& filter, const Interest& interest) ;
		void onRegisterFailed(const Name& prefix, const std::string& reason);
		bool is_finish(){ return this->finish; }
		void start();
		pthread_t get_ptid(){
			return this->m_tid ;
		}
		static void * run(void * para){
			Data_pro * _this = (Data_pro*)para ;
			std::cout << "listening on : " << _this->m_listen_addr 
				<< std::endl;	
			_this->m_face.setInterestFilter(_this->m_listen_addr.data(),
					bind(&Data_pro::onInterest, _this, _1, _2),
					RegisterPrefixSuccessCallback(),
					bind(&Data_pro::onRegisterFailed, _this, _1, _2));
			_this->m_face.processEvents();
			_this->finish = true ;
		}
};

#endif 
