con: consumer.cpp
	g++ -o consumer consumer.cpp -std=c++14 -lndn-cxx -lboost_system
pro: producer.cpp
	g++ -o producer producer.cpp -std=c++14 -lndn-cxx -lboost_system
gen: generate_id.cpp
	g++ -o generate_id generate_id.cpp -std=c++14 -lndn-cxx -lboost_system -lpthread
send: send.cpp
	g++ -o send send.cpp -std=c++14 -lndn-cxx -lboost_system -lpthread
get: getPbk.cpp
	g++ -o getPbk getPbk.cpp -std=c++14 -lndn-cxx -lboost_system -lpthread

clean:
	rm send recv
