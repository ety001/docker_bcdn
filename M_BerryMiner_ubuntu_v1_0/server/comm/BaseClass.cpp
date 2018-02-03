#include <time.h>
#include "BaseClass.h"
#include <typeinfo>


CBaseClassStatistics::CBaseClassStatistics()
    : m_llHoldCnt(0), m_iAddCnt(0), m_iDecCnt(0)
{

}

CBaseClassStatistics::CBaseClassStatistics(const string &strClassName)
    : m_strClassName(strClassName), m_llHoldCnt(0), m_iAddCnt(0), m_iDecCnt(0)
{

}

void CBaseClassStatistics::Add()
{
    m_llHoldCnt++;
    m_iAddCnt++;
}

void CBaseClassStatistics::Dec()
{
    m_llHoldCnt--;
    m_iDecCnt++;
}

string CBaseClassStatistics::Dump()
{
    char buf[MAX_STATIC_LEN] = {0};
    snprintf(buf, MAX_STATIC_LEN-1, "%ld,%s,%ld,%d,%d\n", time(NULL), m_strClassName.c_str(), m_llHoldCnt, m_iAddCnt, m_iDecCnt);
    return string(buf);
}

void CBaseClassStatistics::Clear()
{
    m_iAddCnt = 0;
    m_iDecCnt = 0;
}

string CBaseClassStatistics::DumpClear()
{
    string strDump = Dump();
    Clear();
    return strDump;
}

CBaseClassMgr *CBaseClassMgr::s_pcBaseClassMgr = NULL;
CBaseClassMgr *CBaseClassMgr::Instance()
{
    if (s_pcBaseClassMgr == NULL)
    {
        s_pcBaseClassMgr = new CBaseClassMgr();
    }

    return s_pcBaseClassMgr;
}

void CBaseClassMgr::Add(const string &strClassName)
{
    map<string, CBaseClassStatistics>::iterator iter = m_mapStatistics.find(strClassName);
    if (iter == m_mapStatistics.end())
    {
        CBaseClassStatistics cStatistics(strClassName);
        m_mapStatistics[strClassName] = cStatistics;
    }
    m_mapStatistics[strClassName].Add();
}

void CBaseClassMgr::Dec(const string &strClassName)
{
    map<string, CBaseClassStatistics>::iterator iter = m_mapStatistics.find(strClassName);
    if (iter == m_mapStatistics.end())
    {
        CBaseClassStatistics cStatistics(strClassName);
        m_mapStatistics[strClassName] = cStatistics;
    }
    m_mapStatistics[strClassName].Dec();
    
}

string CBaseClassMgr::Dump()
{
    string strDump;

    map<string, CBaseClassStatistics>::iterator iter = m_mapStatistics.begin();
    map<string, CBaseClassStatistics>::iterator iter_end = m_mapStatistics.end();
    for (; iter != iter_end; iter++)
    {
        CBaseClassStatistics &cStatistics = iter->second;
        strDump += cStatistics.Dump();
    }

    return strDump;
}

void CBaseClassMgr::Clear()
{
    map<string, CBaseClassStatistics>::iterator iter = m_mapStatistics.begin();
    map<string, CBaseClassStatistics>::iterator iter_end = m_mapStatistics.end();
    for (; iter != iter_end; iter++)
    {
        CBaseClassStatistics &cStatistics = iter->second;
        cStatistics.Clear();
    }
}

string CBaseClassMgr::DumpClear()
{
    string strDump;

    map<string, CBaseClassStatistics>::iterator iter = m_mapStatistics.begin();
    map<string, CBaseClassStatistics>::iterator iter_end = m_mapStatistics.end();
    for (; iter != iter_end; iter++)
    {
        CBaseClassStatistics &cStatistics = iter->second;
        strDump += cStatistics.DumpClear();
    }

    return strDump;
}


