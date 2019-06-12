#include <iostream>
#include "eclient.h"

using namespace std;

int main()
{
	EClient eclient("misiyu@b.com");
	int n ;
	cin >> n ;
	if(n == 0)
		eclient.send_email("111@qq.com" , 1 , "123@b.com,misiyu@b.com,hello\n" , 0);
	else if (n ==1)
		eclient.get_email_list("misiyu@b.com","123");
	
	else
		eclient.recv_email("misiyu@b.com","123",0) ;


	return 0;
}
