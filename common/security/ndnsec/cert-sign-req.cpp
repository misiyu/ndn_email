#include <iostream>
#include <ndn-cxx/security/transform/base64-encode.hpp>
#include <ndn-cxx/security/transform/buffer-source.hpp>
#include <ndn-cxx/security/transform/public-key.hpp>
#include <ndn-cxx/security/transform/stream-sink.hpp>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/security/v2/additional-description.hpp>
#include <ndn-cxx/util/io.hpp>

using namespace std;
using namespace ndn;

int main()
{
	Name name("/wgh/identity/004") ;
	security::v2::KeyChain keyChain ;	
	security::Identity identity ;
	security::Key key ;

	identity = keyChain.getPib().getIdentity(name) ;

	key = identity.getDefaultKey() ;

	security::v2::Certificate certificate ;
	Name certificateName = key.getName() ;

	certificateName.append("cert-request").appendVersion() ;

	certificate.setName(certificateName) ;

	// set metainfo
	certificate.setContentType(tlv::ContentType_Key) ;
	certificate.setFreshnessPeriod(100_h) ;

	// set content 
	certificate.setContent(key.getPublicKey().data(), key.getPublicKey().size()) ;

	// set signature-info
	SignatureInfo signatureInfo ;
	auto now = time::system_clock::now() ;
	signatureInfo.setValidityPeriod(security::ValidityPeriod(now, now + 10_days));


	keyChain.sign(certificate, security::SigningInfo(key)
			.setSignatureInfo(signatureInfo)) ;
	cout << certificate << endl ;
	std::ofstream ofs("004.cert-req") ;
	io::save(certificate, ofs) ;

	return 0;
}
