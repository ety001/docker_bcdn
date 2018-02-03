//============================================================
// BaseClass.h : BaseClass.h function declaration
//			(source from INTERNET)
//                          
//============================================================

#ifndef __BASECLASS_H_20171017_15
#define  __BASECLASS_H_20171017_15

#include <string>
#include <map>
#include <typeinfo>

#include "comm/common.h"

#define MAX_STATIC_LEN  1024

using namespace std;

class CBaseClassMgr;
extern CBaseClassMgr *g_pcBaseClassMgr;

class CBaseClassStatistics
{
public:
    CBaseClassStatistics();
    CBaseClassStatistics(const string &strClassName);

    void Add();
    void Dec();
    string Dump();
    void Clear();
    string DumpClear();
protected:
private:
    string m_strClassName;
    _i64 m_llHoldCnt;
    _i32 m_iAddCnt;
    _i32 m_iDecCnt;
};

class CBaseClassMgr
{
public:
    static CBaseClassMgr *Instance();

    void Add(const string &strClassName);
    void Dec(const string &strClassName);
    string Dump();
    void Clear();
    string DumpClear();
protected:
private:
    map<string, CBaseClassStatistics> m_mapStatistics;
    static CBaseClassMgr *s_pcBaseClassMgr;
};

#define CDNNEW(c, para...) true ? ({ \
    c *pc = new c(para); \
    if (pc != NULL) { \
        CBaseClassMgr::Instance()->Add(typeid(*pc).name()); \
    } \
    (c *)pc; \
}) : 0

#define CDNDEL(c) do{ \
    if (c != NULL) { \
        CBaseClassMgr::Instance()->Dec(typeid(*c).name()); \
        delete c; \
    } \
}while(0)

class CBaseClass
{
public:
    virtual void f(){};
private:
};


#endif
