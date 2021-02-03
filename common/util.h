#ifndef _UTIL_H_
#define _UTIL_H_
#include <ndn-cxx/face.hpp>

class Util
{
public:
	static std::string getUsernameFromInterest(const ndn::Interest& interest);
};

#endif 
