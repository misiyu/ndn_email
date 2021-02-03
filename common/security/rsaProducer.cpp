#include <ndn-cxx/face.hpp>
#include <iostream>

#include "crypto.h"
using namespace ndn ;
using namespace std ;
class Producer 
{
	public:
		void run()
		{
			m_face.setInterestFilter("/ndn/edu/pkusz/OA",
					bind(&Producer::onInterest, this, _1, _2),
					RegisterPrefixSuccessCallback(),
					bind(&Producer::onRegisterFailed, this, _1, _2));
			m_face.processEvents();
		}

	private:
		void onInterest(const InterestFilter& filter, const Interest& interest)
		{
			std::cout << "<< I: " << interest << std::endl;
			string intereParam((const char*)
					interest.getApplicationParameters().value(),
					interest.getApplicationParameters().value_size()) ;

			string plainText = mCrypto.rsa_pri_decrypt(intereParam,
					"./rsa_private.key") ;

			cout << plainText << endl ;

			string reply = "红藕香残玉簟秋。轻解罗裳，独上兰舟。云中谁寄锦书来，雁字回时，月满西楼。\n花自飘零水自流。一种相思，两处闲愁。此情无计可消除，才下眉头，却上心头。" ;

			string encodeReply = mCrypto.aes_encryptwithpkcs5padding(reply,"1234567890abcdef") ;

			Data data ;
			data.setName(interest.getName()) ;
			data.setContent((const uint8_t*)encodeReply.data(),
					encodeReply.size()) ;
			m_keyChain.sign(data) ;
			m_face.put(data) ;

		}


		void onRegisterFailed(const Name& prefix, const std::string& reason)
		{
			std::cerr << "ERROR: Failed to register prefix \""
				<< prefix << "\" in local hub's daemon (" << reason << ")"
				<< std::endl;
			m_face.shutdown();
		}

	private:
		Face m_face;
		KeyChain m_keyChain;
		Crypto mCrypto ;
};


int main(int argc, char** argv)
{
	Producer producer;
	try {
		producer.run();
	}
	catch (const std::exception& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
	return 0;
}
