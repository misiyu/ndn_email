#include <ndn-cxx/face.hpp>
#include <iostream>
using namespace ndn ;
using namespace std ;
class Producer : noncopyable
{
	public:
		Producer(){
			const ndn::security::Identity m_identity = m_keyChain.getPib().
				getIdentity(Name("/wgh/identity/002")) ;
			m_keyChain.setDefaultIdentity(m_identity);
		}
		void run()
		{
			std::cout << "pro start " << std::endl ;
			m_face.setInterestFilter("/localhost/nfd/aaa",
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

			static const std::string content = "HELLO KITTY";

			shared_ptr<Data> data = make_shared<Data>();
			data->setName(dataName);
			data->setFreshnessPeriod(0_s); // 10 seconds
			// 数据缓存在节点中，立即变旧
			data->setContent(reinterpret_cast<const uint8_t*>(content.data()), 
					content.size());
			cout << "data->content.value_size = " << 
				data->getContent().value_size() << endl ; 

			m_keyChain.sign(*data);
			cout << "======================" << endl ;
			cout << data->getSignature().getKeyLocator() << endl ;
			cout << "======================" << endl ;

			std::cout << ">> D: " << *data << std::endl;
			cout << data->getMetaInfo() << endl ;
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
