#include <iostream>
#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/security/v2/additional-description.hpp>
#include <ndn-cxx/util/io.hpp>
//#include <ndn-cxx/security/verification-helpers.hpp>

using namespace std;
using namespace ndn;

int main()
{

	string requestFile = "004.cert-req";
	string notBeforeStr = "20191001000000";
	string notAfterStr =  "20291001000000" ;
	vector<string> infos ;
	Name signId("/wgh/identity/001") ;
	string issuerId = "NA";

	security::v2::AdditionalDescription additionalDescription ;

	time::system_clock::TimePoint notBefore = time::fromIsoString(
			notBeforeStr.substr(0,8) + "T" + notBeforeStr.substr(8,6)) ; 
	time::system_clock::TimePoint notAfter = time::fromIsoString(
			notAfterStr.substr(0,8) + "T" + notAfterStr.substr(8,6)) ; 

	security::v2::KeyChain keyChain ;
	
	//shared_ptr<security::v2::Certificate> cetRequest ;
	security::v2::Certificate 
		certRequest = *(io::load<security::v2::Certificate>(requestFile)) ;
	
	//cout << certRequest.getIssuerId() << endl ;
	
	security::v2::Certificate cert ;
	Name certName = certRequest.getKeyName() ;
	certName.append(issuerId).appendVersion() ;

	cert.setName(certName) ;
	cert.setContent(certRequest.getContent()) ;

	cert.setFreshnessPeriod(1_h) ;

	SignatureInfo signatureInfo ;
	signatureInfo
		.setValidityPeriod( security::ValidityPeriod(notBefore,notAfter) ) ;
	if (!additionalDescription.empty()) {
		signatureInfo.appendTypeSpecificTlv(additionalDescription.wireEncode());
	}

	security::Identity signIdentity = keyChain.getPib().getIdentity(signId) ;

	keyChain.sign(cert, security::SigningInfo(signIdentity).
			setSignatureInfo(signatureInfo)) ;
	cout << cert << endl ;

	std::ofstream ofs("004_signby001.cert") ;
	io::save(cert, ofs) ;
	return 0;
}
