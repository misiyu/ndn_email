#include <ndn-cxx/face.hpp>
#include <iostream>
using namespace ndn ;
class Producer : noncopyable
{
	public:
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

			// Create new name, based on Interest's name
			Name dataName(interest.getName());
			dataName
				.append("testApp") // add "testApp" component to Interest name
				.appendVersion();  // add "version" component (current UNIX timestamp in milliseconds)

			static const std::string content = "HELLO KITTY";

			// Create Data packet
			shared_ptr<Data> data = make_shared<Data>();
			data->setName(dataName);
			data->setFreshnessPeriod(20_s); // 10 seconds
			// 数据缓存在节点中，立即变旧
			
			data->setContent(reinterpret_cast<const uint8_t*>(content.data()), content.size());


			// Sign Data packet with default identity
			m_keyChain.sign(*data);
			// m_keyChain.sign(data, <certificate>);

			// Return Data packet to the requester
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
