#include <iostream>
#include <ndn-cxx/face.hpp>
#include <ndn-cxx/util/string-helper.hpp>
#include "util.h"

using namespace std;

string Util::getUsernameFromInterest(const ndn::Interest& interest){

	if(interest.getName().size() < 3) return "" ;
	string username = "" ;
	ndn::SignatureInfo signInfo(interest.getName()
			[ndn::signed_interest::POS_SIG_INFO].blockFromValue());
	username = signInfo.getKeyLocator().getName().toUri();
	cout << "before substr : " << username << endl ;
	username = username.substr(1,username.find("/KEY") - 1) ;
	cout << "before unescape user : " << username << endl ;
	username = ndn::unescape(username) ;	// recover from % encode
	cout << "user : " << username << endl ;
	return username ;
}
