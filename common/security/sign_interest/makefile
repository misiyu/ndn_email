con: consumer.cpp
	g++ -o consumer consumer.cpp -std=c++14 -lndn-cxx -lboost_system
pro: producer.cpp
	g++ -o producer producer.cpp -std=c++14 -lndn-cxx -lboost_system
recv: recv.cpp
	g++ -o recv recv.cpp -std=c++14 -lndn-cxx -lboost_system -lpthread
send: send.cpp
	g++ -o send send.cpp -std=c++14 -lndn-cxx -lboost_system -lpthread

clean:
	rm send recv
