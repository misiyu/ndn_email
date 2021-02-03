#include <iostream>
#include "ndn-cxx/encoding/buffer-stream.hpp"
#include "ndn-cxx/security/transform.hpp"
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/security/v2/additional-description.hpp>
#include <ndn-cxx/util/io.hpp>

//#include "util.hpp"

using namespace std;
using namespace ndn;


bool isValidBlock(const string& str){
	namespace t = ndn::security::transform;
	try{
		OBufferStream os;
		std::istringstream is(str) ;
		t::streamSource(is) >> t::stripSpace("\n") >> t::base64Decode(false)
			>> t::streamSink(os);
		Block b(os.buf()) ;
	}catch(...){
		return false ;
	}
	return true ;
}

int main()
{
	std::stringstream ss ;
	string str =  "hellodhnajhfjdhafhdashfudh9238u9u89hudbsuvgh786y78yheubhdkjsbha" ;
	string str1 =  "Bv0CqwcnCAU1NTU1NQgDS0VZCAgm4PfKp1IrPQgEc2VsZggJ/QAAAW8SdpYUFAkYAQIZBAA27oAV/QEmMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAzbN3Y9xsiFoUYb2mzamhMiiuR/cFYcmok7w2ttpRTbZ26ayXYDUF5LwYFOwB7ab5fDylYAq8tWPrkI/0Bv4iWMUXS9AthKHNGy7AcFd5JIROb/d1PGsOQnyJy53lVzNC85CVytQA5OLQRBtJZYUajHupc9kvWRMnXSwNZ+rDJ441OFPMvCpB2X9OPK6yug3AEXhPsRQOf2XrvKYmuEqoQlaiT+BtmVhkkPjc7/9vsq4kKB5BaDoU2Iqz406+p6Gs1rKHVskPPVrfKN7fRrn6Prz8C5u696Z4FFFcqead4z1q2XIoirNFLODCD5F//5lAe9/ZF5V/spvjzve/7rj5UQIDAQABFkcbAQEcGAcWCAU1NTU1NQgDS0VZCAgm4PfKp1IrPf0A/Sb9AP4PMjAxOTEyMTdUMDYwNjAw/QD/DzIwMzkxMjEyVDA2MDU1ORf9AQCkw2gGGWmAhvtLm4faK9HE/7zBQ+0VOFajknHnzZD4mCZauc8zEO19r+8OTitmVASfUow6Cz6cUMxd6l2JOc8ZmhH0ucPD8a9liN5AcUj7CfV+likJcYBln7Ita5B4bODVMSb5c9lr9He4Nz3LdAbTycUwjFYdWfjK+TC9AwLvkOV09G+TFmFtHMWQvNeHEDXtxYstlSbt1T8uUbsQ08rK9ynwh2oAUjXy7bEkjoZV/1iNhpWH7gjodxo/uOgG+j2y8XB1qn8AbRl9EVPucdefLPuk9vGQ3uPAjiGRfiftob9RHs96U+RkdcyY86f5dpnpNMEoGk4QisP8Ga41IKw0" ;
	ss << str1 ;
	if(!isValidBlock(str1)){
		cout << "this string can not transform to block" << endl ;
		return 0 ;
	}

	security::v2::Certificate cert ;
	try{
		cert = *(io::load<security::v2::Certificate>(ss)) ;
		cout << cert << endl ;
	}catch(...){
		cout << "error - =======================" << endl ; 
	}
	cout << "end" << endl ;

	return 0;
}
