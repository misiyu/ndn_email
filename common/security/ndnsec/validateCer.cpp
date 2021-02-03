#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/v2/validator.hpp>
#include <ndn-cxx/security/verification-helpers.hpp>
#include <iostream>
#include <sstream>
#include <ndn-cxx/util/io.hpp>


using namespace::ndn ;
using namespace::std ;


int main(int argc, char** argv)
{
	KeyChain m_keyChain ;
	string certFile = "./004_signedby_002.cer" ;
	//string certFile = "./004.cert-req.cer" ;
	security::v2::Certificate 
		cert = *(io::load<security::v2::Certificate>(certFile)) ;

	// 保存证书到string流
	std::stringstream ss ;
	io::save(cert,ss) ;
	//cout << ss.str() << endl ;
	
	// 从string流中加载证书
	security::v2::Certificate 
		certTest = *(io::load<security::v2::Certificate>( ss )) ;
	cout << certTest << endl ;

	//cert.getSignature()
	cout << cert.getIdentity() << endl ;
	cout << cert.getKeyName() << endl ;
	cout << cert.getIssuerId() << endl ;

	const SignatureInfo& sigInfo = cert.getSignature().getSignatureInfo() ;
	cout << sigInfo.hasKeyLocator() << endl ;
	cout << sigInfo.getKeyLocator().getName() << endl ;
	string keyPath = sigInfo.getKeyLocator().getName().toUri() ;
	int idx = keyPath.find("KEY",0) ;
	string idName = keyPath.substr(0,idx-1) ;
	cout << "idName : " << idName << endl ;

	//const security::Key& signerKey

	//bool ret = ndn::security::verifySignature(cert , m_keyChain.getPib().
			//getIdentity(Name("/wgh/identity/003")).getDefaultKey()) ;
	bool ret = ndn::security::verifySignature(cert , m_keyChain.getPib().
			getIdentity(Name(idName.data())).getKey(Name(keyPath.data()))) ;
	cout << "verify result = " << ret << endl ;
	return 0;
}
