#可以多个目录空格隔开
COMMONDIR      :=comm/
#可以多个目录空格隔开
SERVERDIR      :=src/

BINDIR         :=bin/
SERVER_TARGET  :=bcdn_server
SRC_SUFFIX     :=cpp c cc

    #external
EXTERN_FLAGS   :=-D_REENTRANT -D_GNU_SOURCE
EXTERN__LIB    :=

####以下是可选的编译参数
    #------use logger(-DLOGGER) or not
LOG            :=-DLOGGER
    #------use logsys(LOGSYS=-DLOGSYS) or log4cpp(LOG4CLIB=-llog4cplus)
#LOGSYS         :=-DLOGSYS
#LOG4CLIB       :=-llog4cplus
#LOG4CLIB       :=-llog4cplus
    #------debug mode or not(-DNDEBUG)
#-march=nocona just for Intel(R) Xeon(R) CPU
RELEASE        :=-DNDEBUG -O3 -mmmx -msse -msse2 -msse3 -funit-at-a-time -frename-registers -fforce-addr -finline-limit=2000 -freorder-blocks -freorder-functions -march=nocona -mtune=nocona -maccumulate-outgoing-args -minline-all-stringops
DEBUG          :=-D_DEBUG
    #------use zlib or not
#ZIP           :=-lz
    #------use openssl(-lcrypto) or not
CRYPTO         :=-lcrypto
    #------use mysql(-lmysqlclient_r -lz) or not
#MYSQL          :=-lmysqlclient_r -lz
MYSQL          :=-lz
    #------use epoll(-DUSEEPOLL) or poll
#EPOLL          :=-DUSEEPOLL
  #program versions
DATE           :=$(shell date +"%Y%m%d")
TIME           :=$(shell date +"%H%M%S")
HVERSION       :=-DPROGRAM_VERSION_HIGH=$(DATE)
LVERSION       :=-DPROGRAM_VERSION_LOW=1$(TIME)-1000000
PROGRAMVERSION :=$(SERVERDIR)ProgramVersion

CC             :=$(CXX)
INCLUDE        :=-I./ -I/usr/local/include -I/usr/local/protobuf/include -I/usr/local/include

CXXFLAGS       :=-Wall -g -pg -std=c++11 $(INCLUDE) $(DEBUG) $(EPOLL) $(EXTERN_FLAGS)
LIB            :=-L/usr/lib -L/usr/local/lib -L./lib -lstdc++ -L/usr/local/protobuf/lib -lpthread -lrt $(MYSQL) $(LOG4CLIB) $(ZIP) $(CRYPTO) $(EXTERN__LIB)
CFLAGS         :=$(CXXFLAGS)

####以下是一些有用的函数

#下面用$(1)代替$@的原因是，在规则依赖里，$@不起作用

#得到所有SRC_SUFFIX后缀的源代码文件
GETSRC=$(foreach dir,$(1),$(wildcard $(addprefix $(dir)*.,$(SRC_SUFFIX))))

#得到.o文件
GETOBJ=$(foreach suffix,$(SRC_SUFFIX),$(filter %.o,$($(1):.$(suffix)=.o)))

####以下是工程创建规则，请勿盲目修改

SERVERSRC:=$(call GETSRC,$(SERVERDIR))
COMMONSRC:=$(call GETSRC,$(COMMONDIR))

SERVEROBJ:=$(call GETOBJ,SERVERSRC)
COMMONOBJ:=$(call GETOBJ,COMMONSRC)

SERVERDEP:=$(SERVEROBJ:.o=.d)
COMMONDEP:=$(COMMONOBJ:.o=.d)

CXXFLAGS+=-MD

ULTIMATE_TARGET:=$(SERVER_TARGET).$(DATE).$(TIME)

all: $(BINDIR)$(SERVER_TARGET)

link: all
	@echo $(SERVER_TARGET) -\> $(ULTIMATE_TARGET)
	@mv $(BINDIR)$(SERVER_TARGET) $(BINDIR)$(ULTIMATE_TARGET)
	@ln -sf $(ULTIMATE_TARGET) $(BINDIR)$(SERVER_TARGET)

$(BINDIR)$(SERVER_TARGET) : $(COMMONOBJ) $(SERVEROBJ)
	$(CXX) $^ $(CXXFLAGS) -o $@ $(LIB) -lprotobuf -levent


$(PROGRAMVERSION).o : force
	$(CXX) $(CXXFLAGS) -c -o $@ $*.cpp $(HVERSION) $(LVERSION)

force : ;

commonobj : $(COMMONOBJ)

serverobj : $(SERVEROBJ)

cleanobj : 
	$(RM) $(foreach dir,$(COMMONDIR),$(dir)*.o)
	$(RM) $(foreach dir,$(SERVERDIR),$(dir)*.o)

cleandep : 
	$(RM) *.d
	$(RM) $(foreach dir,$(COMMONDIR),$(dir)*.d)
	$(RM) $(foreach dir,$(SERVERDIR),$(dir)*.d)

cleandist : cleanobj cleandep

clean : cleandist
	$(RM) $(BINDIR)$(SERVER_TARGET)

love: clean all

.PHONY : all link force commonobj serverobj cleanobj cleandep cleandist clean love  

sinclude $(SERVERDEP) $(COMMONDEP)


