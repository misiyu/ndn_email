#include <fstream>
#include "eclient.h"

string get_ndn_addr(string filepath){
	string result = "" ;
	std::ifstream infile ;
	infile.open(filepath.data());
	if(!infile.is_open()){
		cout << "file " << filepath << " open error!" << endl ;
		exit(1);
	}
	getline(infile,result) ;
	cout << "host addr : " << result << endl ;
	infile.close();
	return result ;
}

EClient::EClient(string e_addr){
	this->m_e_addr = e_addr ;
	this->m_ndn_addr = get_ndn_addr("./ndn_addr.conf") ;
	cout << "my ndn addr : " << this->m_ndn_addr << endl ;
}


inline string i2str(int n){
	std::stringstream ss ;
	ss << n ;
	return ss.str();
}

void EClient::m_expressInterest(string interest_name , 
		void(EClient::*onData)(const Interest &, const Data&)){
	ndn::Interest interest(ndn::Name(interest_name.data()));
	interest.setInterestLifetime(2_s);
	this->m_face.expressInterest(
			interest, 
			bind(&EClient::onData,this,_1,_2),
			bind(&EClient::onNack,this,_1,_2),
			bind(&EClient::onTimeout,this,_1));
	std::cout << "expressInterest : " << interest.getName() << std::endl;
}

string EClient::ea2na(const string e_addr){
	return "/ndn/edu/pkusz/gdcni19/emailserver" ;
	//return "/localhost/nfd/emailserver" ;
}


int EClient::send_email(const string e_addr , int e_addr_type
		, const string message , int message_type) {
	string d_addr = e_addr ;
	char * send_data ;
	int data_len ;
	int slice_sum ;
	if(e_addr_type == 1) {  // email eg. 123@pku.edu.cn
		d_addr = this->ea2na(e_addr);
	}
	if(message_type == 1){  // filepath
		unsigned long file_sz = get_file_size(message.data(),slice_sum);
		send_data = (char*)malloc(sizeof(char)*file_sz);
		data_len = file_sz ;
		FILE *infile = fopen(message.data(),"rb");
		if(infile == NULL){
			cout << "can not open file " << message << endl ;
			exit(1);
		}
		fread(send_data,sizeof(char),file_sz,infile);
		fclose(infile);
	}else{
		data_len = message.length();
		send_data = (char*)malloc(sizeof(char)*data_len);
		memcpy(send_data , message.data() , data_len);
		slice_sum = data_len/(P_DATA_SZ-2) ;
		if(data_len % (P_DATA_SZ-2) != 0) slice_sum ++ ;
	}
	Data_pro m_data_pro(send_data , data_len , this->m_ndn_addr);
	m_data_pro.start();
	
	// interest format /d_addr/(p|g)-src_addr-slice_sum
	std::stringstream ss ;
	ss << slice_sum << "-" << getSystemTime();
	string interest_name = d_addr+"/p-"+this->m_ndn_addr+"-"+ ss.str();
	//string interest_name = d_addr+"/&p" ;
	this->m_expressInterest(interest_name , &EClient::onData);
	this->m_face.processEvents(time::milliseconds::zero(),true);
	pthread_join(m_data_pro.get_ptid(),NULL);
	return 0 ;
}

int EClient::get_file_sz(const string e_addr , string key, int file_id){
	string interest_name  = "" ;
	if(file_id == -1) {  // email list size
		// interest format /d_addr/sl-e_addr ;
		interest_name = this->ea2na(e_addr)+"/"+std::to_string(getSystemTime())
			+"/ls-"+e_addr ;
	}else{
	// interest format /d_addr/&sf&file_id ;
		interest_name = this->ea2na(e_addr)+"/"+std::to_string(getSystemTime())+
			"/fs-"+ std::to_string(file_id) ;
	}
	this->m_expressInterest(interest_name , &EClient::onData);
	this->m_face.processEvents(time::milliseconds::zero(),true);
	return 0 ;
}

int EClient::recv_email(const string e_addr , string key , int file_id){
	this->get_file_sz(e_addr,key,file_id);
	return 0 ;
}

int EClient::get_email_list(const string e_addr , string key){
	this->get_file_sz(e_addr , key , -1);
	string e_list_interest = this->ea2na(e_addr)+"/"+e_addr	;

	return 0 ;
}


void EClient::onData(const Interest& interest , const Data& data){
	string interest_name = interest.getName().toUri();
	std::cout << "\n\n143:recv data : " << interest_name << std::endl ;
	int tmp1 = interest_name.find_first_of('-');
	string n_addr ;

	if(interest_name.at(tmp1-1) == 's'){ // get size
		char buff[1000];
		memcpy(buff , (char*)(data.getContent().value()) , 1000 );
		int16_t addr_len = 0 ;
		memcpy(&addr_len , buff , sizeof(int16_t));
		memcpy(&this->slice_sum , buff+sizeof(int16_t) , sizeof(int));
		char addr_buff[addr_len];
		memcpy(addr_buff,buff+sizeof(int16_t)+sizeof(int),addr_len);
		n_addr = addr_buff ;
		cout << "144 : "<<slice_sum << endl ;
		cout << "n_addr :" << n_addr << endl ;
		this->m_face.shutdown();
		Data_base db ;
		Recv_data recv_data(n_addr,slice_sum,db) ;
		recv_data.start();
		pthread_join(recv_data.get_ptid(),NULL);
	}else{

		this->m_face.shutdown();
	}
}
void EClient::onNack(const Interest& interest, const lp::Nack& nack){
	cout << "no route to " << interest.getName() << endl;
	this->m_face.shutdown();
}
void EClient::onTimeout(const Interest& interest){
	cout << "Time out " << interest.getName() << endl ;
	Interest interest_new(interest.getName());
	this->m_face.expressInterest(interest_new,
			bind(&EClient::onData,this,_1,_2),
			bind(&EClient::onNack,this,_1,_2),
			bind(&EClient::onTimeout,this,_1));
	cout << "expressInterest : " << interest_new.getName() << endl ;
}

