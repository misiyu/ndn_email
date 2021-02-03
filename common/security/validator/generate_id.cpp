#include <ndn-cxx/face.hpp>
#include <iostream>
using namespace ndn ;
using namespace std ;

int main(int argc, char** argv)
{
	
	KeyChain m_keyChain ;
	m_keyChain.createIdentity(Name("/wgh/identity/004")) ;

	return 0;
}
