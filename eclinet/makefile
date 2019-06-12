COM_DIR=../common
INC_DIR= -I${COM_DIR} -I.

SRCS := $(wildcard *.cpp) \
		$(wildcard ${COM_DIR}/*.cpp)
#OBJS := $(SRCS:.cpp=.o)   # replace .cpp by .o
OBJS := ${patsubst %.cpp, %.o, ${SRCS}}

CC := g++ 
CFLAGS := -g -std=c++14 ${INC_DIR}
LFLAGS := -lpthread -lpcap -lndn-cxx -lboost_system -lboost_thread -ljsoncpp
TARGET := ndn_email_c # name of the file where the main function in . 


$(TARGET):$(OBJS)  
	$(CC) ${notdir ${OBJS}} -o $@  $(LFLAGS)

${OBJS}:${SRCS}
	$(CC) $(CFLAGS) -c $? 


clean:  
	rm -rf $(TARGET) *.o  

test:
	echo $(SRCS)
	echo $(OBJS)
	echo $<
	echo $(CFLAGS)
