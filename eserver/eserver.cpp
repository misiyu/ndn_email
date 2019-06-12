#include "eserver.h"

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

EServer::EServer(){
	for (int i = 0; i < 20; i++) {
		this->rd_pool[i] =NULL;
		this->dp_pool[i] =NULL;
	}
	this->m_listen_addr = get_ndn_addr("./ndn_addr.conf");
	cout << "listening on : " << this->m_listen_addr << endl ;

	this->m_face.setInterestFilter(this->m_listen_addr.data(),
			bind(&EServer::onInterest, this, _1, _2),
			RegisterPrefixSuccessCallback(),
			bind(&EServer::onRegisterFailed, this, _1, _2));
	this->m_face.processEvents();
}

EServer::~EServer(){

}
int EServer::start_a_rdt(string cli_ndn_addr , int slice_sum){
	int i = 20;
	cout << "hello" << endl ;
	while(i >= 20 ){
		cout << "hello" << endl ;
		for (i = 0; i < 20; i++) {
			cout << "hello : " << i << endl ;
			if(this->rd_pool[i] == NULL){
				this->rd_pool[i] = new Recv_data(cli_ndn_addr,slice_sum,
						this->m_db);
				this->rd_pool[i]->start();
				break ;
			}else if(this->rd_pool[i]->is_finish() == true){
				delete(this->rd_pool[i]);
				this->rd_pool[i] = new Recv_data(cli_ndn_addr,slice_sum,
						this->m_db);
				this->rd_pool[i]->start();
				break ;
			}
		}
	}
	return i ;
}

int EServer::start_a_dpt(const char * send_data , int data_len , string ndn_addr){
	int i = 20 ;
	while(i >= 20){
		for (i = 0; i < 20; i++) {
			if(this->dp_pool[i] == NULL || this->dp_pool[i]->is_finish()){
				if(this->dp_pool[i]!=NULL) delete(this->dp_pool[i]);
				ndn_addr += std::to_string(i);
				cout << "66: " << data_len << endl ;
				this->dp_pool[i] = new Data_pro(send_data , data_len , ndn_addr);
				break;
			}
		}
	}
	return i;
}

void EServer::onInterest(const InterestFilter& filter, const Interest& interest) {

	// interest format /d_addr/(p|g)-src_addr-slice_sum
	string interest_name = interest.getName().toUri();
	char respon_buff[1000];
	cout << "recv_interest : " << interest_name << endl ;
	int tmp1 = interest_name.find_first_of('-');
	int tmp2 = interest_name.find('-',tmp1+1);
	int tmp3 = interest_name.find('-',tmp2+1);
	if(interest_name.at(tmp1-1) == 'p'){ // put data
		tmp1 += 1 ;
		std::stringstream ss ;
		ss << interest_name.substr(tmp2+1 , tmp3-tmp2-1);
		int slice_sum = 0 ;
		ss >> slice_sum ;
		string cli_ndn_addr = interest_name.substr(tmp1 , tmp2-tmp1);
		cout << "client at " << cli_ndn_addr << " want to put data " << endl ;
		this->start_a_rdt(cli_ndn_addr,slice_sum);
	}else if(interest_name.at(tmp1-1) == 's'){ // get size
		if(interest_name.at(tmp1-2) == 'l'){ // get email list size
			// interest format /d_addr/time/ls-e_addr ;
			string email_addr = interest_name.substr(tmp1+1 , 
					interest_name.length()-tmp1-1);
			string e_list ;
			email_addr =  get_raw_string(email_addr)  ;
			cout << email_addr << endl ;
			int data_len = this->m_db.get_e_list(email_addr , e_list);
			cout << data_len << endl ;
			cout << e_list << endl ;
			int slice_sum = data_len/(P_DATA_SZ-2) ;
			if(data_len %(P_DATA_SZ-2) != 0) slice_sum ++ ;
			int tmp = this->start_a_dpt(e_list.data(),e_list.length(),
					this->m_listen_addr) ;
			this->dp_pool[tmp]->start();
			string n_addr = this->m_listen_addr + std::to_string(tmp);
			int16_t r_b_sz = n_addr.length() ;
			memcpy(respon_buff , &r_b_sz , sizeof(int16_t));
			memcpy(respon_buff+sizeof(int16_t) , &slice_sum , sizeof(int));
			memcpy(respon_buff+sizeof(int16_t)+sizeof(int) , n_addr.data() ,
				  n_addr.length());
		}else if(interest_name.at(tmp1-2) == 'f'){ // get email
			// interest format /d_addr/time/fs-file_id ;
			string id_str = interest_name.substr(tmp1+1 , 
					interest_name.length()-tmp1-1);
			int file_id = std::stoi(id_str);
			cout << "file_id : " << file_id << endl ;
			int buff_len = this->m_db.get_file_sz(file_id);
			char * buff = (char*)malloc(sizeof(char)*buff_len);

			int slice_sum = this->m_db.read_file(buff,file_id,buff_len);
			cout << "123 buff_len : " << buff_len << endl ;
			cout << "123 slice_sum : " << slice_sum << endl ;

			cout << "126 buff : " << buff << endl ;
			int tmp = this->start_a_dpt(buff, buff_len, this->m_listen_addr) ;
			this->dp_pool[tmp]->start();
			string n_addr = this->m_listen_addr + std::to_string(tmp);
			int16_t r_b_sz = n_addr.length() ;
			memcpy(respon_buff , &r_b_sz , sizeof(int16_t));
			memcpy(respon_buff+sizeof(int16_t) , &slice_sum , sizeof(int));
			memcpy(respon_buff+sizeof(int16_t)+sizeof(int) , n_addr.data() ,
				  n_addr.length());
			if(buff!=NULL) free(buff);
		}
	}
	Data data ;
	data.setName(interest_name);
	data.setContent(reinterpret_cast<const uint8_t*>(respon_buff),1000);
	this->m_keyChain.sign(data) ;
	this->m_face.put(data);
}

void EServer::onRegisterFailed(const Name& prefix, const std::string& reason){
	std::cerr << "ERROR: Failed to register prefix \""
		<< prefix << "\" in local hub's daemon (" << reason << ")"
		<< std::endl;
	m_face.shutdown();
}
