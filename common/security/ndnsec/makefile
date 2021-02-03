sign-req: cert-sign-req.cpp
	g++ -o cert-sign-req cert-sign-req.cpp -std=c++14 -lndn-cxx -lboost_system -lpthread
cert-gen: cert-gen.cpp
	g++ -o cert-gen cert-gen.cpp -std=c++14 -lndn-cxx -lboost_system
validate: validateCer.cpp
	g++ -o validateCer validateCer.cpp -std=c++14 -lndn-cxx -lboost_system
signbuf: signbuf.cpp
	g++ -o signbuf signbuf.cpp -std=c++14 -lndn-cxx -lboost_system -lpthread
delid: delete_id.cpp
	g++ -o delete_id delete_id.cpp -std=c++14 -lndn-cxx -lboost_system -lpthread

clean:
	rm send recv
