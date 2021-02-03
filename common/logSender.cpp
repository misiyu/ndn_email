#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cmath>

#include "logSender.h"

using namespace std;

int LogSender::interestSeq = 0 ;

static string getFormatTime(){
	time_t tt;
	::time( &tt );
	tt = tt + 8*3600;  // transform the time zone
	tm* t= gmtime( &tt );
	char buf[30] ;
	memset(buf, 0,30) ;

	sprintf(buf,"%d-%02d-%02d %02d:%02d:%02d",
			t->tm_year + 1900,
			t->tm_mon + 1,
			t->tm_mday,
			t->tm_hour,
			t->tm_min,
			t->tm_sec);
	return buf ;
}

static long getTimestamp(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_sec;
}
static string getTimestampStr(){
	return to_string(getTimestamp()) ;
}

static double getCosine(const vector<long> & v1, const vector<long> & v2){
	double a = 0, b = 0 , c = 0 ;
	for (int i = 0; i < v1.size(); ++i) {
		a += v1[i]*v2[i] ;
		b += v1[i]*v1[i] ;
		c += v2[i]*v2[i] ;
	}
	return a/(sqrt(b) * sqrt(c)) ;
}
void LogSender::loadConfig() {
	string file_path = "./config.json" ;
	Json::Reader reader ;
	Json::Value root ;
	std::ifstream in(file_path.data(),ios::binary);
	if(!in.is_open()){
		cout << "can not open file " << file_path << endl ;
		exit(1);
	}
	reader.parse(in,root);
	in.close();
	if(!root.isMember("misLogPrefix") ){
		printf("please configure log srever prefix\n");
		this->state = false ;
		return ;
	}
	string logServerPrefix ;
	if(root["misLogPrefix"].size() == 0){
		logServerPrefix = root["misLogPrefix"].asString() ;
		logServerPrefix += "/" + to_string(getTimestamp()) ;
		logServerPrefixs.push_back(logServerPrefix) ;
		cout << "log server prefix : " << logServerPrefix << endl ;
	}else{
		for (int i = 0; i < root["misLogPrefix"].size(); ++i) {
			logServerPrefix = root["misLogPrefix"][i].asString() ;
			logServerPrefix += "/i/" + to_string(getTimestamp()) ;
			logServerPrefixs.push_back(logServerPrefix) ;
			cout << "log server prefix : " << logServerPrefix << endl ;
		}
	}
}

LogSender::LogSender (){
	//pthread_cond_init(&mcond, NULL) ;
	//pthread_mutex_init(&mutex, NULL) ;
	this->state = true ;

	loadConfig() ;
}
LogSender::~LogSender (){

}


int LogSender::isRepeate(const string & action, const long &time, int level) {
	int ret = 1 ;
	bool hasBlank = false ;
	vector<long> actionV(130) ;
	for (int i = 4; i < action.size() - 8; ++i) {
		uint8_t c = action[i] ;
		c = c % 130 ;
		actionV[c] ++ ;
		if(c == ' ') hasBlank = true ;
	}
	if(!hasBlank) return ret ;

	auto it = logV.begin() ;
	cout << action << endl ;
	for (; it != logV.end() ; it ++ ) {
		while(time - it->back() > 30) {
			it = logV.erase(it);
			if(it == logV.end()) break ;
		}
		if(it == logV.end()) break ;
		double cos = getCosine(actionV, *it) ;
		cout << "cos ======================== " << cos << endl ;
		if(cos > 0.9) {
			if(level > 0 && it->back() < time){ ret = 0 ; }
			it->back() = time ;
			return ret ;
		}else if(cos > 0.85){
			if( time - it->back() < 20 ){ return ret ; }
			it->back() = time ;
			return 0 ;
		}else if(cos > 0.7){
			if( time - it->back() < 10 ){ return ret ; }
			it->back() = time ;
			return 0 ;
		}
	}

	actionV.push_back(time) ;
	if(it == logV.end()){
		logV.push_back(actionV) ;
		return 0 ;
	}
	return ret ;
}

int LogSender::addLog(const string& prefix,const string &action, 
		const string& username ,const string& sig, const int& level){
	if(action.size() < 20) return 0 ;
	long tSec = getTimestamp() ;	
	int repeat = isRepeate(action, tSec, level) ;
	if(repeat) return 1 ;
	Json::Value root ;
	root["Type"] = "network" ;
	root["Command"] = "Log" ;
	root["Prefix"] = prefix ;
	root["Action"] = action ;
	root["Sig"] = username ;
	root["Username"] = username ;
	root["Level"] = level ;
	root["Timestamp"] = to_string(tSec) ;
	if(root.toStyledString().size() > 7000) return 0 ;
	logQue.push_back(root.toStyledString()) ;


	return 0 ;

}

int LogSender::addLog(const string& userId,int sockfd, const string& data){
	return addLog(userId,sockfd,data.data(),data.size()," no more") ;
}


int LogSender::addLog(const string& userId,int sockfd, const char* data, 
		int dataLen,const string & username, const string &sig ){

	struct sockaddr_in peerAddr ;
	socklen_t len = sizeof(peerAddr) ;
	int ret = getpeername(sockfd, (struct sockaddr*)(&peerAddr), &len) ;
	if(ret < 0) return -1;
	string peerIp = inet_ntoa(peerAddr.sin_addr) ;
	int peerPort = ntohs(peerAddr.sin_port);

	string action;
	if(peerPort == 443){
		action = userId+" send/recv from/to " + peerIp + ":"+
			to_string(peerPort) + " dataLen : " + to_string(dataLen);
		cout << "发送日志 ： "  << action << endl ;
	}else{
		int i = 0 ;
		for (i = 0; i < dataLen && i < 80; ++i) {
			if(!isprint(data[i])) break ;
		}
		string dataStr(data, i)  ;
		action = dataStr ;

	}

	addLog(userId,action,username) ;
	return 0 ;
}

void LogSender::start(){
	pthread_t tid ;
	pthread_create(&tid, NULL, LogSender::run, this) ;
	pthread_create(&tid, NULL, LogSender::minThread, this) ;
}
int LogSender::sendLog() {
	while(logQue.empty()){
		sleep(1) ;
	}
	sendInterest(logQue.front()) ;
	logQue.pop_front() ;

}
/*
 * brief : send interest packet 
 * param : logStr , json format string of a log 
 * ret : null 
 */ 
void LogSender::sendInterest(const string& logStr){


	Interest interest ;
	interest.setMustBeFresh(true) ;

	interest.setApplicationParameters((const uint8_t*)logStr.data(),
			logStr.size()) ;	

	for (int i = 0; i < logServerPrefixs.size(); ++i) {
		Name name(logServerPrefixs[i]) ;
		name.appendSequenceNumber(interestSeq++ ) ;
		interest.setName(name) ;
		mKeyChain.sign(interest) ;
		mface.expressInterest(interest,
				bind(&LogSender::onData, this,  _1, _2),
				bind(&LogSender::onNack, this, _1, _2),
				bind(&LogSender::onTimeout, this, _1));
		cout << "send log : " << logStr<< " " << interest << endl ; 
	}

}
void LogSender::onData(const Interest& interest, const Data& data)
{
	//std::cout << "send log success" << std::endl;
}

void LogSender::onNack(const Interest& interest, const lp::Nack& nack)
{
	//std::cout << "received Nack with reason " << nack.getReason()
	//<< " for interest " << interest << std::endl;
}

void LogSender::onTimeout(const Interest& interest)
{
	std::cout << "Timeout " << interest << std::endl;
}

void* LogSender::minThread(void* param) {
	//cout << "满地黄花堆积" << endl ;
	((LogSender*)param)->mface.processEvents(time::milliseconds::zero(),true);
}

void* LogSender::run(void* param){
	LogSender * _this = (LogSender*)param ;
	while(_this->state){
		_this->sendLog() ;
	}
}
