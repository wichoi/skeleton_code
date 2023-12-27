EXEC = p520-client

CXXFLAGS = -I. -I./FSPCoreAPI/api \
	-I./FSPCoreAPI/src -I./FSPCoreAPI/src/linux\
	-I$(ROOTDIR)/lib/curl-7.31.0/include \
	-I$(ROOTDIR)/lib/openssl-1.0.2u/include \
	-I$(ROOTDIR)/lib/cJSON \
	-I$(ROOTDIR)/lib/fw-manager \
#-I$(ROOTDIR)/lib/include \
#CXXFLAGS += -fpermissive

EXTRA_CFLAGS = -DDEBUG_ENABLE -DFSP_PLATFORM_LINUX
LDFLAGS = -ldl -lpthread -lstdc++ -lrt -lm -rdynamic
LDLIBS = -L$(ROOTDIR)/lib/lib -lssl -lcrypto -lcurl -lcjson -lfwmgr

FSP_LIBS = \
	FSPCoreAPI/src/Linux/FSPLinuxFileFinder.cpp \
	FSPCoreAPI/src/Linux/FSPLinuxSysInfo.cpp \
	FSPCoreAPI/src/FSPApplication.cpp \
	FSPCoreAPI/src/FSPCoreAPI.cpp \
	FSPCoreAPI/src/FSPDateTime.cpp \
	FSPCoreAPI/src/FSPDateTimeSpan.cpp \
	FSPCoreAPI/src/FSPDebugManager.cpp \
	FSPCoreAPI/src/FSPEvent.cpp \
	FSPCoreAPI/src/FSPEventMessageFramework.cpp \
	FSPCoreAPI/src/FSPEventQueueManager.cpp \
	FSPCoreAPI/src/FSPFile.cpp \
	FSPCoreAPI/src/FSPFileFinder.cpp \
	FSPCoreAPI/src/FSPFileInfo.cpp \
	FSPCoreAPI/src/FSPFileSplitInfo.cpp \
	FSPCoreAPI/src/FSPLazyTimerThread.cpp \
	FSPCoreAPI/src/FSPMemFile.cpp \
	FSPCoreAPI/src/FSPMutex.cpp \
	FSPCoreAPI/src/FSPRefObject.cpp \
	FSPCoreAPI/src/FSPSimpleFactoryPattern.cpp \
	FSPCoreAPI/src/FSPSingletonManager.cpp \
	FSPCoreAPI/src/FSPSysInfo.cpp \
	FSPCoreAPI/src/FSPSystemTimerThread.cpp \
	FSPCoreAPI/src/FSPTask.cpp \
	FSPCoreAPI/src/FSPTaskEvent.cpp \
	FSPCoreAPI/src/FSPTaskManager.cpp \
	FSPCoreAPI/src/FSPThread.cpp \
	FSPCoreAPI/src/FSPTimer.cpp \
	FSPCoreAPI/src/FSPTimerThread.cpp \
	FSPCoreAPI/src/FSPWatchdogManager.cpp \
	FSPCoreAPI/src/MSString.cpp

#SRCS = $(wildcard *.cpp)
SRCS = \
	main.cpp \
	log.cpp \
	event.cpp \
	timer.cpp \
	utils.cpp \
	cli.cpp \
	monitor.cpp \
	config-manager.cpp \
	json-manager.cpp \
	config-handler.cpp \
	curl-handler.cpp \
	fwup-handler.cpp \
	cloud-handler.cpp \
	auth-handler.cpp \
	main-client.cpp

SRCS += $(FSP_LIBS)

#OBJS = $(SRCS:.cpp=.o)

.PHONY: all romfs clean

all: $(EXEC)

$(EXEC): $(SRCS)
	$(CXX) $(CXXFLAGS) $(EXTRA_CFLAGS) $(LDFLAGS) -o $@ $(SRCS) $(LDLIBS)

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.o

