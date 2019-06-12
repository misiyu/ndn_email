#include "data_pro.h"

Data_pro::Data_pro(const char * send_data , int data_len  ,string listen_addr){

	this->m_data_buff = (char*)malloc(data_len);
	memcpy(this->m_data_buff , send_data , data_len);
	this->m_data_len = data_len ;
	this->m_listen_addr = listen_addr ;
	this->slice_sum = data_len/(P_DATA_SZ-2) ; 
	if(data_len % (P_DATA_SZ-2) != 0) this->slice_sum++;
	this->s_data_c = 0;
	this->finish = false ;

}
Data_pro::~Data_pro(){
	free(this->m_data_buff);
}

void Data_pro::start(){
	std::cout << "data pro start " << std::endl ;
	int ret = pthread_create(&(this->m_tid) , NULL , run , (void*)this);
	std::cout << "create thread code :  " << ret << std::endl ;
}

void Data_pro::onInterest(const InterestFilter& filter, const Interest& interest) {
	string interest_name = interest.getName().toUri();
	std::cout << "data_pro revc interest : " << interest_name << std::endl ;
	int tmp1 = interest_name.find_last_of('/');
	std::stringstream ss ;
	ss << interest_name.substr(tmp1+1 , interest_name.length()-tmp1-1);
	int slice_n ;
	ss >> slice_n ;
	char content[P_DATA_SZ];
	int start_i = slice_n*(P_DATA_SZ-2) ;
	if( start_i > this->m_data_len or slice_n < 0){
		if(slice_n == this->slice_sum){
			Data data;
			data.setName(interest_name);
			char c = 'o';
			data.setContent(reinterpret_cast<const uint8_t*>(&c),1);
			this->m_keyChain.sign(data);
			this->m_face.put(data);
		}
		std::cout << "slice no error " << std::endl ;
		this->m_face.shutdown();
		return ;
	}
	int16_t data_size = 0 ;
	if(this->m_data_len - start_i > P_DATA_SZ-2) data_size = P_DATA_SZ-2 ;
	else data_size = this->m_data_len - start_i ;
	memcpy(content , &data_size , sizeof(int16_t));
	memcpy(content+2 , this->m_data_buff+start_i , data_size);
	Data data;
	data.setName(interest_name);
	data.setContent(reinterpret_cast<const uint8_t*>(content),P_DATA_SZ);
	this->m_keyChain.sign(data);
	this->m_face.put(data);
	this->s_data_c ++ ;
	//if(this->s_data_c >= this->slice_sum) this->m_face.shutdown();
}

void Data_pro::onRegisterFailed(const Name& prefix, const std::string& reason)
{
	std::cerr << "ERROR: Failed to register prefix \""
		<< prefix << "\" in local hub's daemon (" << reason << ")"
		<< std::endl;
	m_face.shutdown();
}
