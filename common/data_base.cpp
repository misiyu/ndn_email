
#include "data_base.h"

Data_base::Data_base(){
	this->m_e_dir = "./edatabase/";
	this->m_table = "./edatabase/e_table";
	this->m_e_id_file = "./edatabase/e_cur_id" ;
	this->cur_id = 0;

	FILE *file_p = fopen(this->m_table.data(),"rb");
	if(file_p == NULL){
		//fclose(file_p);
		file_p = fopen(this->m_table.data(),"wb");
	}
	fclose(file_p);
	file_p = fopen(this->m_e_id_file.data() , "r");
	if(file_p == NULL){
		file_p = fopen(this->m_table.data(),"w");
		fprintf(file_p,"%d",this->cur_id);
	}else{
		fscanf(file_p , "%d" , &(this->cur_id));
	}
	fclose(file_p);
}

Data_base::~Data_base(){

}


int Data_base::get_e_list(string e_addr , string& e_list){
	int len = 0;
	
	std::ifstream infile(this->m_table.data(), std::ios::binary);
	if(!infile.is_open()){
		std::cout << "can not find  " << this->m_table << std::endl;
		exit(1);
	}
	string line ;
	while(getline(infile,line))
	{
		if(line.find(e_addr,0) != string::npos){
			e_list += line+'\n' ; 
			len += line.length()+1;
		}
	}
	
	return len ;
}

string i2str(int n){
	std::stringstream ss ;
	ss << n ;
	return ss.str();
}

int Data_base::save(char *buff , int buff_sz , int file_id){
	string filepath = this->m_e_dir+"email-"+i2str(file_id);
	FILE *file = fopen(filepath.data(),"wb");
	fwrite(buff , sizeof(char) , buff_sz , file);
	fclose(file);
	return 0 ;
}

int Data_base::append(char * buff , int buff_sz , int file_id){
	string filepath = this->m_e_dir+"email-"+i2str(file_id);
	FILE *file = fopen(filepath.data(),"r");
	if(file == NULL){
		this->append_table(buff,buff_sz,file_id);
	}else{
		fclose(file);
	}
	file = fopen(filepath.data() , "ab+");
	fseek(file, 0 , SEEK_END);
	fwrite(buff , sizeof(char) , buff_sz , file);
	fclose(file);
	return 0 ;
}
int Data_base::read_file(char * buff , int file_id , int & file_sz){
	string filepath = this->m_e_dir+"email-"+i2str(file_id);
	int file_slice_n = 0 ;
	get_file_size(filepath.data() , file_slice_n);
	//buff = (char*)malloc(file_sz);
	FILE *file = fopen(filepath.data(),"rb");
	fread(buff , sizeof(char) , file_sz , file);
	fclose(file);
	return file_slice_n ;
}

int Data_base::get_file_sz(int file_id){
	string filepath = this->m_e_dir+"email-"+i2str(file_id);
	int file_slice_n = 0 ;
	int file_sz = get_file_size(filepath.data() , file_slice_n);
	return file_sz ;
}

int Data_base::append_table(char *buff , int buff_sz , int file_id){
	//fileid,sa,da,title\n
	std::cout << "Data_base : append_table : " << buff << ":" << file_id
		<< std::endl ;
	int i ;
	for (i = 0; i < buff_sz; i++) {
		if(buff[i] == '\n') break ;
	}
	buff[i] = '\n';
	FILE *file = fopen(this->m_table.data(),"ab+");
	fseek(file, 0 , SEEK_END);
	fprintf(file , "%d,", file_id);
	fwrite(buff , sizeof(char) , i+1 , file);
	fclose(file) ;
	return 0 ;
}
