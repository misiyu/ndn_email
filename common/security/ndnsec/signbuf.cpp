#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/v2/validator.hpp>
#include <ndn-cxx/security/transform/public-key.hpp>
#include <ndn-cxx/security/verification-helpers.hpp>
#include <ndn-cxx/security/signing-helpers.hpp>
#include <iostream>
#include <sstream>
#include <ndn-cxx/util/io.hpp>
using namespace ndn ;
using namespace std ;


int main(int argc, char** argv)
{
	uint8_t buff[20] ;
	memset(buff,'a',sizeof(buff)) ;

	KeyChain m_keyChain;

	security::SigningInfo signinfo = security::signingByIdentity(Name("/wgh/identity/002")) ;

	Block sign = m_keyChain.sign(buff,sizeof(buff), signinfo) ;
	cout << sign << endl ;

	sign.encode() ;
	cout << "hasWire = " << sign.hasWire() << endl ; 
	std::stringstream ss ;
	io::saveBlock(sign , ss) ;
	cout << sign.type() << endl; 
	cout << ss.str() << endl ;

	optional<Block> sign1 =  io::loadBlock(ss) ;
	cout << "sign1.type = " << sign1->type() << endl ;

	cout << signinfo << endl ;

	security::v2::PublicKey pKey ;

	const Buffer PublicKeyBits = m_keyChain.getPib()
		.getIdentity(Name("/wgh/identity/002")).getDefaultKey()
		.getPublicKey();

	pKey.loadPkcs8(PublicKeyBits.data(), PublicKeyBits.size()) ;

	
	bool ret = ndn::security::verifySignature(buff , sizeof(buff) ,
			sign1->value(), sign1->value_size() ,
			pKey) ;
	cout << "verify result = " << ret << endl ;

	return 0;
}
