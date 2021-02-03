#ifndef _LOGSENDER_H_
#define _LOGSENDER_H_

#include <ndn-cxx/face.hpp>

using namespace ndn;
using std::string ;
using std::deque ;
using std::vector ;

class Log{
public:
	Log(const string &userId, int sockfd, const string & data):
		userId(userId), sockfd(sockfd), data(data){
	}
	string userId ;
	int sockfd ;
	string data ;

} ;
class LogSender
{
public:
	LogSender ();
	~LogSender ();
	int addLog(const string& prefix,const string &action,const string& username, 
			const string& sig="", const int& level =0);
	int addLog(const string& userId,int sockfd, const string& data);

	int addLog(const string& userId,int sockfd, const char* data, int dataLen,
			const string& username  , const string& sig = "");
	void start();
private:
	void loadConfig() ;
	int sendLog() ;
	void sendInterest(const string& logStr);

	void onData(const Interest& interest, const Data& data) ;
	void onNack(const Interest& interest, const lp::Nack& nack) ;
	void onTimeout(const Interest& interest) ;

	static void* run(void* param) ;
	static void* minThread(void* param) ;

	int isRepeate(const string & action, const long &time, int level) ;

private:
	/* data */
	//string logServerPrefix ;
	vector<string> logServerPrefixs ;
	deque<string> logQue ;
	//pthread_cond_t mcond ;
	//pthread_mutex_t mutex ;
	bool state ;
	static int interestSeq ;
	Face mface ;

	ndn::KeyChain mKeyChain ;
	std::list<std::vector<long>> logV;
};

#endif 
