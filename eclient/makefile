COM_DIR=../common
INC_DIR= -I${COM_DIR} -I.

SRCS := $(wildcard *.cpp) \
		$(wildcard ${COM_DIR}/*.cpp)

#OBJS := ${patsubst %.cpp, %.o, ${SRCS}}
OBJS := ndn_email_c.o eclient.o data_base.o data_pro.o recv_data.o

CC := g++ 
CFLAGS := -g -std=c++14 ${INC_DIR}
LFLAGS := -lpthread -lpcap -lndn-cxx -lboost_system -lboost_thread -ljsoncpp
TARGET := ndn_email_c # name of the file where the main function in . 


$(TARGET): ${OBJS}
	$(CC) ${OBJS} -o ${TARGET}  $(LFLAGS)

ndn_email_c.o : ndn_email_c.cpp eclient.h
	${CC} $(CFLAGS) -c ndn_email_c.cpp

eclient.o:eclient.cpp eclient.h
	${CC} $(CFLAGS) -c eclient.cpp

data_base.o : ../common/data_base.cpp ../common/data_base.h
	${CC} $(CFLAGS) -c ../common/data_base.cpp

data_pro.o : ../common/data_pro.cpp ../common/data_pro.h
	${CC} $(CFLAGS) -c ../common/data_pro.cpp

recv_data.o : ../common/recv_data.cpp ../common/recv_data.h
	${CC} $(CFLAGS) -c ../common/recv_data.cpp


clean:  
	rm -rf $(TARGET) *.o  

test:
	echo $(SRCS)
	echo $(OBJS)
	echo $<
	echo $(CFLAGS)
