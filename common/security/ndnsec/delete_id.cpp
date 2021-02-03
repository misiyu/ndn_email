#include <ndn-cxx/face.hpp>
#include <iostream>
using namespace ndn ;
using namespace std ;

int main(int argc, char** argv)
{
	
	KeyChain m_keyChain ;
	m_keyChain.deleteIdentity( 
			m_keyChain.getPib().getIdentity(Name("/hello"))  ) ;
	

	return 0;
}
