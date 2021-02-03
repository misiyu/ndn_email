rsapro: rsaProducer.cpp
	g++ -g -o rsapro rsaProducer.cpp crypto.cpp -std=c++14 -lndn-cxx -lboost_system -lcrypto
aespro: aesProducer.cpp
	g++ -g -o aespro aesProducer.cpp crypto.cpp -std=c++14 -lndn-cxx -lboost_system -lcrypto
con: consumer.cpp
	g++ -g -o consumer consumer.cpp  crypto.cpp -std=c++14 -lndn-cxx -lboost_system -lcrypto
pro: producer.cpp
	g++ -g -o producer producer.cpp crypto.cpp -std=c++14 -lndn-cxx -lboost_system -lcrypto

clean:
	rm send recv
