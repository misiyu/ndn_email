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
		Consumer(){
			
		}
		void run()
		{
			Interest interest(Name("/localhost/nfd/aaa"));
			//Interest interest(Name("/test/nfd/aaa"));
			interest.setInterestLifetime(2_s); // 2 seconds
			interest.setMustBeFresh(true);
			//interest.setCanBePrefix(true) ;

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
			char buff[100] ;
			memset(buff, 0 ,sizeof(buff)) ;
			std::cout << data << std::endl;
			//std::cout << data.getContent() << std::endl ;
			memcpy(buff, data.getContent().value(),
					data.getContent().value_size()) ;
			

			//bool ret = ndn::security::verifySignature(data , m_keyChain.getPib().
					//getIdentity(Name("/wgh/identity/002")).getDefaultKey()) ;

			bool ret = ndn::security::verifySignature(data , m_keyChain.getPib().
					getIdentity(Name("/wgh/identity/002")).getDefaultKey()
				.getDefaultCertificate());

			cout << "verify result = " << ret << endl ;

			cout << buff << endl ;
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
		//ndn::security::v2::Validator m_validator ;
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
