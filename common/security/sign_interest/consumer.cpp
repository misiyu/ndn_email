#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/v2/validator.hpp>
#include <ndn-cxx/security/verification-helpers.hpp>
#include <iostream>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
	// Additional nested namespaces can be used to prevent/limit name conflicts

using namespace::ndn ;
using namespace::std ;

class Consumer : noncopyable
{
	public:

		bool chkIsValidBlock(const uint8_t* buf, size_t bufSize){

			const uint8_t* pos = buf;
			const uint8_t* const end = buf + bufSize;

			//uint32_t m_type = tlv::readType(pos, end);
			uint64_t length = tlv::readVarNumber(pos, end);
			// pos now points to TLV-VALUE

			//BOOST_ASSERT(pos <= end);
			if (length > static_cast<uint64_t>(end - pos)) {
				cout << "======================" << endl ;
				return false ;
			}
			return true ;
		}
		void run()
		{
			Interest interest(Name("/localhost/nfd/aaa"));
			//Interest interest(Name("/test/nfd/aaa"));
			interest.setInterestLifetime(1_s); // 2 seconds
			interest.setMustBeFresh(true);
			
			//char value[] = "hello" ;
			//Block block = makeBinaryBlock(tlv::Content, value, 5);
			//interest.setApplicationParameters(block) ;
			cout << interest.hasApplicationParameters() << endl ;
			
			const Name& interestName = interest.getName() ;

			
			bool ret = ndn::security::verifySignature(interest,m_keyChain.getPib().
					getIdentity(Name("/wgh1")).getDefaultKey()
				.getDefaultCertificate());
			cout << "verify result = " << ret << endl ;

			//signed_interest::MIN_SIZE == 2 
			// 倒数第一个为签名，倒数第二个为签名信息
			if (interestName.size() < signed_interest::MIN_SIZE) {
				cout << "no signatrue" << endl ;
			}
			m_keyChain.sign(interest) ;

			ret = ndn::security::verifySignature(interest,m_keyChain.getPib().
					getIdentity(Name("/wgh1")).getDefaultKey()
				.getDefaultCertificate());
			cout << "verify result = " << ret << endl ;


			if (interestName.size() < signed_interest::MIN_SIZE) {
				cout << "no signatrue" << endl ;
			}

			//cout << interestName[signed_interest::POS_SIG_INFO].blockFromValue().value_size() << endl ;

			try{

				const Block& signinfoBlock = 
					interestName[signed_interest::POS_SIG_INFO].blockFromValue();
				//cout << "valid block > :"<< chkIsValidBlock(interestName[signed_interest::POS_SIG_INFO]
						//.value(), interestName[signed_interest::POS_SIG_INFO]
						//.value_size()) << endl ;
				//Block b(interestName[signed_interest::POS_SIG_INFO].value(),
						//interestName[signed_interest::POS_SIG_INFO].value_size()) ;
				SignatureInfo signInfo(signinfoBlock) ;
				//SignatureInfo signInfo(b) ;
				cout << "sign type =  "<< signInfo.getSignatureType() << endl ;
				// sign type 在 signing-info.hpp中定义
				cout << "signInfo>>>>>>>>: " << signInfo.getKeyLocator().getName().toUri() << endl ;
			}catch(...){
				cout << "hello ,error " << endl ;
				exit(1) ;
			}


			m_face.expressInterest(interest,
					bind(&Consumer::onData, this,  _1, _2),
					bind(&Consumer::onNack, this, _1, _2),
					bind(&Consumer::onTimeout, this, _1));

			std::cout << "Sending " << interest << std::endl;

			// processEvents will block until the requested data received or timeout occurs
			m_face.processEvents();
		}

	private:
		void onData(const Interest& interest, const Data& data)
		{
			std::cout << data.getName().toUri() << std::endl;
			string content((char*)data.getContent().value() , 
					data.getContent().size()-1);
			//cout << "getSignature = " << data.getSignature().getInfo() << endl ;
			cout << "content size = " << data.getContent().size() << endl ;
			cout << content << endl ;
			//std::cout << data.getContent().value() << std::endl ;
		}

		void onNack(const Interest& interest, const lp::Nack& nack)
		{
			std::cout << "received Nack with reason " << nack.getReason()
				<< " for interest " << interest << std::endl;
		}

		void onTimeout(const Interest& interest)
		{
			std::cout << "Timeout " << interest << std::endl;
		}

	private:
		Face m_face;
		KeyChain m_keyChain ;
};


int main(int argc, char** argv)
{
	Consumer consumer;
	try {
		consumer.run();
	}
	catch (const std::exception& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
	return 0;
}
