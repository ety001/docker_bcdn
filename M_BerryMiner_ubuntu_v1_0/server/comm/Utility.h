//============================================================
// Utility.h : interface of utility class for misc functions
//
// Created: 2006-08-28
//============================================================

#ifndef __STRING_UTIL_H_20060828_11
#define  __STRING_UTIL_H_20060828_11

#include <string>
#include <vector>
#include <map>
#include "comm/common.h"
//#include "SDLogger.h"
using namespace std;

class Utility
{
private:
	Utility() { }

	const static char HEX_DATA_MAP[];

	static map<_u32, string> _peerid_maps;

public:
	static const int NONBLOCK_RECV_ERROR = -1;
	static const int NONBLOCK_RECV_AGAIN = -11;
	const static char* SPACE_STRING;
	const static string EMPTY_STRING;

	const static uint8_t ZERO_CID[MAX_CID_LEN];

public:
	static bool check_cid_gcid(const uint8_t *cid, const uint8_t *gcid);
	//static bool delete_alllitte_file(const string file_path);
	
	static _u32 get_limited_peer_num_by_filesize(_u64 filesize);
	static bool byte_to_hex(uint8_t b, char* buf);
	static string bytes_to_hex(const uint8_t* data, int len);	
	static bool hex_text_to_bytes(const string& hex, uint8_t* buffer, int& buffer_len);

	static string trim_string(const string& str);

	static _u64 current_time_ms();
	static _u32 current_time_secs()
	{
		struct timeval now;
		gettimeofday(&now, NULL);
		return (_u32)now.tv_sec;
	}
	static string get_date_string();
	static bool get_date_string(char* buf, int buf_len, time_t unix_time = 0);
	static string get_date_string(time_t unix_time);
	static string get_time_string(time_t unix_time = 0);
	static bool get_time_string(char * buf, int buf_len, time_t unix_time);
	static string get_date_time_string(time_t unix_time = 0);
	static bool get_date_time_string(char* buf, int buf_len, time_t unix_time = 0);
	static int get_hour_int();
	static int get_hour_int(time_t unix_time);

	static bool set_fd_block(int fd, bool block_mode);
	static bool set_socket_tcpcork(int fd, bool turnon);

	static bool set_socket_linger(int fd, bool turnon, int time_out_secs);
	static bool setReuseAddr(int fd);
	
	static bool set_socket_send_timeout(int fd, int timeout_sec);
	static bool set_socket_recv_timeout(int fd, int timeout_sec);

	static int recv_nonblock_data(int sock_fd, char* buffer, int &recv_len);	
	static int send_nonblock_data(int sock_fd, const char* buffer, int bytes_tosend);		
	static int recv_data(int sock_fd, char* buffer, int recv_len);		
	static int send_data(int sock_fd, const char* buffer, int bytes_tosend);			

	static string get_peer_ip(int fd);
	static string get_local_ip(int fd);	
	static _u32 get_peer_ip_int(int fd);	
	static _u32 get_local_ip_int(int fd);		
	static bool read_mac_addr(char* buf, int& buffer_len);
	static string get_local_peerid(unsigned bind_addr, uint16_t listen_port, int index = 0);
	static string get_product_peerid(unsigned bind_addr, uint16_t listen_port);

	static string ip_ntoa(uint32_t ip_num);
	static bool ip_ntoa(uint32_t ip_num, char* buf, int buf_len);
	static string itostr(__int32 num);
	static string itostr(_u32 num);
	static string itostr(__int64 num);
	static string itostr(_u64 num);
	static string filesize_readable_text(_u64 filesize);

	static int SplitString(char *srcStr, const string &delim, vector<string>& strList);
	static int split(const std::string& source, const char* delimitor, std::vector<std::string>& result_array);

	static bool file_exists(const char* file_path, bool exclude_dir = true);
	static bool is_valid_seedfile(const char* file_path, _u64 min_filesize);
	static bool is_valid_seedfile(const struct stat64 &buf, _u64 min_filesize);
	static bool is_directory(const char* file_path, string &realpath);
	static bool is_directory(const char * file_path, const struct stat64 &buf, string &realpath);
	static _u64 retrieve_filesize(const char* file_path);
	static int connect_by_ip_port(const char* server_ip, int server_port);

	static string extract_filename(const char* filepath);
	static string extract_filename_erase_td(const char * filepath);
	static string extract_parent_dir(const char* filepath);

	static bool ends_with(const char* fullstr, const char* substr);

	static void sleep_in_seconds(int seconds);

	static char* strncpy(char* dest, const char* src, int count);

	static bool cacl_threshold_md5_hex(int threshold, _u64 filesize, char* md5hex_buffer);
	static _u32 string_to_hash(const string &str);

	static _u32 get_filelast_mtime(const char *file_path);
	static _u64 get_filesize_by_fd(const int &fd);

	static string get_first_ip_from_domainname(const char *domainname);

	static _u64  get_allign_offset(_u64  offset, uint32_t  allign_base);
	static _u32  get_allign_length( _u32 length,uint32_t  allign_base);

	static _u32   replace_all(std::string& str,  const std::string& pattern,  const std::string& newpat);
#if 0
	static int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t &outlen);
	static int u2g(char *inbuf,size_t inlen,char *outbuf,size_t &outlen);
	static int g2u(char *inbuf,size_t inlen,char *outbuf,size_t &outlen);
#endif
	static uint32_t utf8_decode( char *s, uint32_t *pi );
	static std::string UrlEncode(const std::string& src);
	static std::string UrlDecode(const std::string& src);
	static int ExecShell(const char* sCommand);
	static bool get_httpget_param(const char *buf, map<string, string> &param_list);
	inline static int endian()
	{ 
		static   int   init   =   0; 
		static   int   endian_value; 

		if   (init)   return   endian_value; 
		init   =   1; 
		char   *p   =   (char*)&init; 
		return   endian_value   =   p[0]?0:1; 
	}
	
//	static _u64 get_filesize_by_path(const char *filepath);
private:
	static int hex_char_value(char c)
	{
		if(c >= '0' && c <= '9')
			return c - '0';
		else if(c >= 'A' && c <= 'Z')
			return c - 'A' + 10;
		else if(c >= 'a' && c <= 'z')
			return c - 'a' + 10;
		else
			return -1;
	}

private:
	static string m_buffered_peerid;
	static string m_buffered_p2p50_peerid;

	 //DECL_LOGGER(logger);
	
};


#ifndef   swap64
#define   swap64(x)               ((((x)&0xFF) << 56) \
										| (((x) >> 56) & 0xFF) \
										| (((x)&0xFF00) << 40) \
										| (((x) >> 40) & 0xFF00) \
										| (((x)&0xFF0000) << 24) \
										| (((x) >> 24) & 0xFF0000) \
										| (((x)&0xFF000000) << 8) \
										| (((x) >> 8) & 0xFF000000))
#endif

#ifndef htond
#define   htond(x)   (Utility::endian()?(x):swap64((uint64_t)x)) 
#endif
#ifndef ntohd
#define   ntohd(x)   (Utility::endian()?(x):swap64((uint64_t)x)) 
#endif

#ifndef htonll
#define  htonll(x) (Utility::endian()?(x):swap64((uint64_t)x))
#endif
#ifndef ntohll
#define  ntohll(x) (Utility::endian()?(x):swap64((uint64_t)x))
#endif


#endif // #ifndef __STRING_UTIL_H_20060828_11

