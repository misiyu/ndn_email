#include <iostream>

#include "recv_data.h"

//Recv_data::Recv_data(){
//}

Recv_data::Recv_data(string interest_name , int segment_num , Data_base &db):
	m_db(db){
	this->m_interest_name = interest_name ;
	this->m_segment_num = segment_num+1 ;
	cout << "interest_name : " << this->m_interest_name << endl ;
	cout << "segment_num : " << this->m_segment_num << endl ;
	this->m_get_segment_count = 0;
	this->finish = false ;
	this->m_file_id = db.get_cur_id();
	db.cur_id_p1();
}

Recv_data::~Recv_data(){

}

void Recv_data::start(){
	cout << "start recv data from client" << endl ;
	int ret = pthread_create(&(this->m_ptid) , NULL , run , (void*)this);
	cout << "create thread code : " << ret << endl ;
}


void Recv_data::onData(const Interest& interest , const Data& data){
	this->m_get_segment_count ++ ;
	if(this->m_get_segment_count >= this->m_segment_num ) return ;
	char buff[P_DATA_SZ];
	memcpy(buff , (char*)(data.getContent().value()) , P_DATA_SZ);
	int16_t recv_sz = 0 ;
	memcpy(&recv_sz , buff , sizeof(int16_t));
	this->m_db.append(buff+2,recv_sz,this->m_file_id);
	if(this->m_get_segment_count >= this->m_segment_num){
		this->m_face.shutdown();
		return ;
	}else{
		std::stringstream ss ;
		ss << getSystemTime()<<"/"<< this->m_get_segment_count ;
		string interest_name = this->m_interest_name +"/"+ss.str() ;
		cout << "Recv_data expressInterest : " << interest_name << endl ;
		Interest interest(Name(interest_name.data()));
		interest.setInterestLifetime(2_s);
		this->m_face.expressInterest(interest , 
				bind(&Recv_data::onData,this,_1,_2),
				bind(&Recv_data::onNack,this,_1,_2),
				bind(&Recv_data::onTimeout,this,_1));
		if(this->m_get_segment_count >= this->m_segment_num-1 ){
			cout << "shutdown" << endl ;
			this->m_face.shutdown();
		}
	}
}

void Recv_data::onNack(const Interest& interest, const lp::Nack& nack){
	cout << "no route to " << interest.getName() << endl;
	this->m_face.shutdown();
}

void Recv_data::onTimeout(const Interest& interest){
	if(this->m_get_segment_count == this->m_segment_num-1) { 
		this->m_face.shutdown();
		return ;
	}	
	cout << "Time out " << interest.getName() << endl ;
	Interest interest_new(interest.getName());
	this->m_face.expressInterest(interest_new,
			bind(&Recv_data::onData,this,_1,_2),
			bind(&Recv_data::onNack,this,_1,_2),
			bind(&Recv_data::onTimeout,this,_1));
	cout << "expressInterest : " << interest_new.getName() << endl ;
}
