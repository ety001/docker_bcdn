#ifndef __THREAD_SAFE_HASHMAP_H_201008271718__
#define __THREAD_SAFE_HASHMAP_H_201008271718__

#include <map>
#include <string>
#include "comm/common.h"
#include "comm/SDMutexLock.h"

using namespace std;

template<class _data>
struct my_null_delete
{
    void operator()(_data &data) const
    {
        return;
    }
};

template<class _data>
struct my_delete
{
    void operator()(_data &data) const
    {
        delete data;
		data = NULL;
        return;
    }
};

template<class _key>
struct hash_value
{
    size_t operator()(const _key &__x) const
    {
        return __x.hash_value();
    }
};

//特化
template<>
struct hash_value<string>
{
    size_t operator()(const string &__x) const
    {
        //BKDR Hash Function
        char *str = (char *)__x.c_str();
        size_t seed = 131; // 31 131 1313 13131 131313 etc..
        size_t hash = 0;
        while (*str)
        {
            hash = hash * seed + (*str++);
        }
        return (hash & 0x7FFFFFFF);
    }
};
template<>
struct hash_value<_u64>
{
    size_t operator()(const _u64 &__x) const
    {
        return (__x);
    }
};
template<>
struct hash_value<_u32>
{
    size_t operator()(const _u32 &__x) const
    {
        return (__x);
    }
};


template < typename Key, typename Data, typename _Delete = my_null_delete<Data> >
class ThreadSafeHashMap
{
public:
    struct stNodeData{
        stNodeData(){}
        stNodeData(Data data)
            :  m_data(data)
        {}
        typename map< Key, stNodeData >::iterator m_prev;
        typename map< Key, stNodeData >::iterator m_next;
        Data m_data;
    };
public:
    typedef map< Key, stNodeData > DataMap;
    typedef typename map< Key, stNodeData >::iterator DataMapIter;
    DataMapIter DataMapIter_NULL;
public:
    ThreadSafeHashMap(_u32 bucket_num, bool use_lru = false, __int64 max_size = 200*1000*1000) 
        : m_bucket_num(bucket_num), m_lru(use_lru), m_max_size(max_size)
    {
        m_bucket_mutex_list = new SDMutexLock[m_bucket_num];
        m_bucket_list = new map<Key, stNodeData>[m_bucket_num];
        m_size = 0;
        m_head = DataMapIter_NULL;
        m_tail = DataMapIter_NULL;
    }
    ~ThreadSafeHashMap()
    {
        delete []m_bucket_mutex_list;
        delete []m_bucket_list;
    }

    bool find(const Key &key)
    {
        bool ret=false;
        _u32 hashvalue = m_hasher(key);
        _u32 bucket_id = hashvalue%m_bucket_num;
        m_bucket_mutex_list[bucket_id].lock();
        DataMapIter iter = m_bucket_list[bucket_id].find(key);
        if(iter != m_bucket_list[bucket_id].end())
        {
            ret = true;         
            if(m_lru)
            {
                m_locker.lock();
                if (m_head != iter) // update the link
                {
                    DataMapIter ptr_prev = iter->second.m_prev;
                    DataMapIter ptr_next = iter->second.m_next;

                    if(ptr_prev != DataMapIter_NULL)
                    {
                        ptr_prev->second.m_next = ptr_next; 
                    }                    
                    iter->second.m_next = m_head;
                    iter->second.m_prev = DataMapIter_NULL;
                    if(m_head != DataMapIter_NULL)
                    {
                        m_head->second.m_prev = iter;
                    }
                    m_head = iter;
                
                    if (ptr_next != DataMapIter_NULL)
                    {
                        ptr_next->second.m_prev = ptr_prev;
                    }
                    if (m_tail == iter)
                    {
                        m_tail = ptr_prev;
                    }
                }
                m_locker.unlock();
            }
        }
        m_bucket_mutex_list[bucket_id].unlock();

        return ret;
    }

	_u32 get_bucketid(const Key &key)
	{
        _u32 hashvalue = m_hasher(key);
        return hashvalue%m_bucket_num;
	}
    //如果返回非空，必须return_data
    Data *get_data(const Key &key)
    {
        Data *ret = NULL;
        _u32 hashvalue = m_hasher(key);
        _u32 bucket_id = hashvalue%m_bucket_num;
        m_bucket_mutex_list[bucket_id].lock();
        DataMapIter iter = m_bucket_list[bucket_id].find(key);
        if(iter != m_bucket_list[bucket_id].end())
        {
            ret = &iter->second.m_data;         
            if(m_lru)
            {
                m_locker.lock();
                if (m_head != iter) // update the link
                {
                    DataMapIter ptr_prev = iter->second.m_prev;
                    DataMapIter ptr_next = iter->second.m_next;

                    if(ptr_prev != DataMapIter_NULL)
                    {
                        ptr_prev->second.m_next = ptr_next; 
                    }                    
                    iter->second.m_next = m_head;
                    iter->second.m_prev = DataMapIter_NULL;
                    if(m_head != DataMapIter_NULL)
                    {
                        m_head->second.m_prev = iter;
                    }
                    m_head = iter;
                
                    if (ptr_next != DataMapIter_NULL)
                    {
                        ptr_next->second.m_prev = ptr_prev;
                    }
                    if (m_tail == iter)
                    {
                        m_tail = ptr_prev;
                    }
                }
                m_locker.unlock();
            }
        }
        else
        {
            m_bucket_mutex_list[bucket_id].unlock();
        }

        return ret;
    }
    
    void return_data(const Key &key)
    {
        _u32 hashvalue = m_hasher(key);
        _u32 bucket_id = hashvalue%m_bucket_num;
        m_bucket_mutex_list[bucket_id].unlock();

        return;
    }

    bool copy_data(const Key &key, Data &data)
    {
        bool ret = false;
        _u32 hashvalue = m_hasher(key);
        _u32 bucket_id = hashvalue%m_bucket_num;
        m_bucket_mutex_list[bucket_id].lock();
        DataMapIter iter = m_bucket_list[bucket_id].find(key);
        if(iter != m_bucket_list[bucket_id].end())
        {
            ret = true;
            data = iter->second.m_data;         
            if(m_lru)
            {
                m_locker.lock();
                if (m_head != iter) // update the link
                {
                    DataMapIter ptr_prev = iter->second.m_prev;
                    DataMapIter ptr_next = iter->second.m_next;

                    if(ptr_prev != DataMapIter_NULL)
                    {
                        ptr_prev->second.m_next = ptr_next; 
                    }                    
                    iter->second.m_next = m_head;
                    iter->second.m_prev = DataMapIter_NULL;
                    if(m_head != DataMapIter_NULL)
                    {
                        m_head->second.m_prev = iter;
                    }
                    m_head = iter;
                
                    if (ptr_next != DataMapIter_NULL)
                    {
                        ptr_next->second.m_prev = ptr_prev;
                    }
                    if (m_tail == iter)
                    {
                        m_tail = ptr_prev;
                    }
                }
                m_locker.unlock();
            }
        }
        m_bucket_mutex_list[bucket_id].unlock();

        return ret;
    }

    void add(const Key &key, const Data &data)
    {
        bool need_erase = false;
        Key  erase_key;
        _u32 delete_bucket_id;
        _u32 hashvalue = m_hasher(key);
        _u32 bucket_id = hashvalue%m_bucket_num;
        m_bucket_mutex_list[bucket_id].lock();
        DataMapIter iter = m_bucket_list[bucket_id].find(key);
        if(iter == m_bucket_list[bucket_id].end())
        {
            stNodeData data_time(data);
            data_time.m_prev = DataMapIter_NULL;
            data_time.m_next = DataMapIter_NULL;
            std::pair< DataMapIter, bool > insert_ret = m_bucket_list[bucket_id].insert(make_pair(key, data_time));
            if(insert_ret.second)
            {
                iter = insert_ret.first;
            }
            m_locker.lock();
            m_size++;
            m_locker.unlock();
        }
        else
        {
        	m_deleter(iter->second.m_data);
            iter->second.m_data = data;
        }
        
        if(m_lru)
        {
            m_locker.lock();
            if (m_head != iter) // update the link
            {
                DataMapIter ptr_prev = iter->second.m_prev;
                DataMapIter ptr_next = iter->second.m_next;

                if(ptr_prev != DataMapIter_NULL)
                {
                    ptr_prev->second.m_next = ptr_next; 
                }
                iter->second.m_next = m_head;
                iter->second.m_prev = DataMapIter_NULL;
                if(m_head != DataMapIter_NULL)
                {
                    m_head->second.m_prev = iter;
                }
                m_head = iter;
            
                if (ptr_next != DataMapIter_NULL)
                {
                    ptr_next->second.m_prev = ptr_prev;
                }
                if (m_tail == iter)
                {
                    m_tail = ptr_prev;
                }
            }
            if(m_tail == DataMapIter_NULL)
            {
                m_tail = iter;
            }
            if (m_size > m_max_size) // remove the lease recent node
            {                
                DataMapIter ptr = m_tail;
                
                delete_bucket_id = m_hasher(ptr->first)%m_bucket_num;
                if(delete_bucket_id == bucket_id)       //分情况处理，防止死锁
                {                    
					Data erase_data = ptr->second.m_data;
					m_tail = ptr->second.m_prev;
					m_tail->second.m_next = DataMapIter_NULL;
                    m_bucket_list[delete_bucket_id].erase(ptr->first);
					m_deleter(erase_data);
                }
                else
                {
                    need_erase = true;
                    erase_key = ptr->first;                    
                }
                m_size--;
            }
            m_locker.unlock();

        }
        m_bucket_mutex_list[bucket_id].unlock();
        if(need_erase)
        {
        	del(erase_key);
        }

        return;
    }

    void del(const Key &key)
    {
        _u32 hashvalue = m_hasher(key);
        _u32 bucket_id = hashvalue%m_bucket_num;
        m_bucket_mutex_list[bucket_id].lock();
        DataMapIter iter = m_bucket_list[bucket_id].find(key);
        if(iter != m_bucket_list[bucket_id].end())
        {
            m_locker.lock();            
            if(m_lru)
            {
                DataMapIter ptr_prev = iter->second.m_prev;
                DataMapIter ptr_next = iter->second.m_next;
                if(ptr_prev != DataMapIter_NULL)
                {
                    ptr_prev->second.m_next = ptr_next;
                }
                if(ptr_next != DataMapIter_NULL)
                {
                    ptr_next->second.m_prev = ptr_prev;
                }
                if(m_tail == iter)
                {
                    m_tail = ptr_prev;
                }
                if(m_head == iter)
                {
                    m_head = ptr_next;
                }
            }
            m_size--;
            m_locker.unlock();
			Data erase_data = iter->second.m_data;
            m_bucket_list[bucket_id].erase(iter);
			m_deleter(erase_data);
        }
        m_bucket_mutex_list[bucket_id].unlock();

        return;
    }


    void update(const ThreadSafeHashMap< Key, Data > & hash_map)
    {
        bool set_tail = false;
        for(int i=0; i<(int)m_bucket_num; i++)
        {
            m_bucket_mutex_list[i].lock();
            __int64 tmp_size = m_bucket_list[i].size();
            m_bucket_list[i] = hash_map.m_bucket_list[i];
            m_locker.lock();
            m_size -= tmp_size;
            m_size += m_bucket_list[i].size();          
            if(m_lru)
            {
                DataMapIter iter = m_bucket_list[i].begin();
                DataMapIter iter_end = m_bucket_list[i].end();
                for(; iter!=iter_end; iter++)
                {
                    if(!set_tail)
                    {                   
                        m_tail = iter;
                        set_tail = true;
                    }
                    iter->second.m_next = m_head;
                    iter->second.m_prev = DataMapIter_NULL;
                    if(m_head != DataMapIter_NULL)
                    {
                        m_head->second.m_prev = iter;
                    }
                    m_head = iter;
                }
            }
            m_locker.unlock();
            m_bucket_mutex_list[i].unlock();
        }
    }

    void copy_bucket(_u32 bucket_id, DataMap &bucket)
    {
    	bucket_id = bucket_id % m_bucket_num;
        m_bucket_mutex_list[bucket_id].lock();
        bucket = m_bucket_list[bucket_id];
        m_bucket_mutex_list[bucket_id].unlock();
    
        return;
    }
	
    void get_bucket(_u32 bucket_id, DataMap *&bucket)
    {
    	bucket_id = bucket_id % m_bucket_num;
        m_bucket_mutex_list[bucket_id].lock();
        bucket = &m_bucket_list[bucket_id];
    
        return;
    }
    void return_bucket(_u32 bucket_id)
    {
        m_bucket_mutex_list[bucket_id].unlock();
    
        return;
    }

    __int64 size()
    {
        return m_size;
    }
public:
    hash_value<Key> m_hasher;
	_Delete m_deleter;
    _u32 m_bucket_num;
    SDMutexLock *m_bucket_mutex_list;
    map< Key, stNodeData > *m_bucket_list;
    __int64 m_size;

    SDMutexLock m_locker;
    bool m_lru;
    __int64 m_max_size;
    DataMapIter m_head;
    DataMapIter m_tail;
};

#endif


