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

			Name dataName(interest.getName());

			//static const std::string content = "HELLO KITTY";
			string ptext = "" ;
			for(int i = 0 ; i < 1098; i++){
				ptext += "a" ;
			}
			
			cout << "ptext size = " << ptext.size() << endl ;
			string content = mCrypto.rsa_pub_encrypt( ptext ,
					"./rsa_public.key");
			cout << "content.size() = " << content.size() << endl ;

			shared_ptr<Data> data = make_shared<Data>();
			data->setName(dataName);
			data->setFreshnessPeriod(0_s); // 10 seconds
			// 数据缓存在节点中，立即变旧
			data->setContent(reinterpret_cast<const uint8_t*>(content.data()), 
					content.size());
			cout << "data->content.value_size = " << 
				data->getContent().value_size() << endl ; 


			m_keyChain.sign(*data);
			std::cout << ">> D: " << *data << std::endl;
			m_face.put(*data);
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
