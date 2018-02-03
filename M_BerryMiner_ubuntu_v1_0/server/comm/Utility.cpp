//============================================================
// Utility.cpp : implementation of utility class for misc functions
//
// Created: 2006-08-28
//============================================================

#include "common.h"
#include <netinet/tcp.h>  // for TCP_CORK
#include <linux/if.h>
#include "Utility.h"
#include "Base64.h"
#include <openssl/md5.h>
#include <netdb.h>
//#include <iconv.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SIZE_KB	1024
#define SIZE_MB	1048576
#define SIZE_GB	1073741824
#define _100_KB	102400
#define _100_MB	104857600
const _u64 _100_GB = 107374182400ULL;

const char* Utility::SPACE_STRING = " \t\r\n";
const string Utility::EMPTY_STRING = string("");
const char Utility::HEX_DATA_MAP[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
string Utility::m_buffered_peerid;
string Utility::m_buffered_p2p50_peerid;

const uint8_t Utility::ZERO_CID[MAX_CID_LEN] = {0};


map<_u32, string> Utility::_peerid_maps;

//IMPL_LOGGER(Utility, logger);

bool Utility::check_cid_gcid(const uint8_t *cid, const uint8_t *gcid)
{
	if(cid == NULL || gcid == NULL)
		return false;

	if(bcmp(cid, ZERO_CID, MAX_CID_LEN) == 0)
		return false;

	if(bcmp(gcid, ZERO_CID, MAX_CID_LEN) == 0)
		return false;

	return true;
}


_u32 Utility::get_limited_peer_num_by_filesize(_u64 filesize)
{
	_u32 tmp = 0;
	if(filesize < 10485760)
    {
    	tmp = 8;
    }
    else if(filesize < 52428800)
    {
        tmp = 8 + (filesize-10485760) / 2097152;
    }
    else
    {
        tmp = 28 + (filesize-52428800) / 5242880;
    }
	
	if(tmp > 40) tmp = 40;
		
	return tmp;
}


bool Utility::byte_to_hex(uint8_t b, char * buf)
{
	if(buf == NULL)
		return false;

	int h1 = (b & 0xF0) >> 4;
	int h2 = b & 0xF;

	buf[0] = HEX_DATA_MAP[h1];
	buf[1] = HEX_DATA_MAP[h2];

	return true;
}

string Utility::bytes_to_hex(const uint8_t * data, int len)
{
	if(data == NULL || len <= 0)
		return "";

	string str(len * 2, '*');
	char buf[2];
	for(int i = 0; i < len; i++)
	{
		byte_to_hex(*(data + i), buf);
		str[i * 2] = buf[0];
		str[i * 2 + 1] = buf[1];
	}

	return str;
}

bool Utility::hex_text_to_bytes(const string & hex, uint8_t * buffer, int & buffer_len)
{
	if(hex.length() % 2 != 0 || buffer == NULL)
		return false;

	int nbytes = hex.length() / 2;
	if(buffer_len < nbytes)
		return false;
	buffer_len = 0;
	
	for(int i = 0; i < nbytes; i++)
	{
		char c1 = hex[i * 2];
		int h1 = hex_char_value(c1);
		if(h1 < 0)
			return false;
				
		char c2 = hex[i * 2 + 1];
		int h2 = hex_char_value(c2);
		if(h2 < 0)
			return false;

		uint8_t b = (uint8_t)((h1 << 4) | h2);
		buffer[i] = b;			
	}
	buffer_len = nbytes;

	return true;
}

string Utility::trim_string(const string & str)
{
	string::size_type bpos = str.find_first_not_of(SPACE_STRING);
	string::size_type epos = str.find_last_not_of(SPACE_STRING);
	
	if(bpos < 0 || epos < 0)
		return EMPTY_STRING;
	else if(bpos == 0 && epos == str.length() - 1)
		return str;
	else
		return str.substr(bpos, epos - bpos + 1);
}

_u64 Utility::current_time_ms()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return (_u64)now.tv_sec * 1000 + now.tv_usec / 1000;	
}

string Utility::get_date_string()
{
	time_t t1 = time(NULL);
	struct tm lt;
	localtime_r(&t1, &lt);

	char timebuf[32];
	sprintf(timebuf, "%04d%02d%02d", (lt.tm_year + 1900), (lt.tm_mon + 1), lt.tm_mday);

	return (string)timebuf;
}

bool Utility::get_date_string(char* buf, int buf_len, time_t unix_time)
{
	if(buf == NULL || buf_len < 9)
		return false;

	time_t t1 = unix_time; 
	if(t1 == 0)
		t1 = time(NULL);
	
	struct tm lt;
	localtime_r(&t1, &lt);

	sprintf(buf, "%04d%02d%02d", (lt.tm_year + 1900), (lt.tm_mon + 1), lt.tm_mday);

	return true;
}

string Utility::get_date_string(time_t unix_time)
{
	time_t t1 = unix_time; 
	if(t1 == 0)
		t1 = time(NULL);
	
	struct tm lt;
	localtime_r(&t1, &lt);

	char timebuf[32];
	sprintf(timebuf, "%04d%02d%02d", (lt.tm_year + 1900), (lt.tm_mon + 1), lt.tm_mday);

	return (string)timebuf;
}

string Utility::get_time_string(time_t unix_time)
{
	time_t t1 = unix_time; //time(NULL);
	if(t1 == 0)
		t1 = time(NULL);
	struct tm lt;
	localtime_r(&t1, &lt);

	char timebuf[32];
	sprintf(timebuf, "%02d%02d%02d", lt.tm_hour, lt.tm_min, lt.tm_sec);

	return (string)timebuf;
}

int Utility::get_hour_int()
{
	time_t t1 =  time(NULL);
	struct tm lt;
	localtime_r(&t1, &lt);

	return lt.tm_hour;
}

int Utility::get_hour_int(time_t unix_time)
{
	time_t t1 =  unix_time;
	struct tm lt;
	localtime_r(&t1, &lt);

	return lt.tm_hour;
}

bool Utility::get_time_string(char * buf, int buf_len, time_t unix_time)
{
	if(buf == NULL || buf_len < 9)
		return false;

	time_t t1 = unix_time; 
	if(t1 == 0)
		t1 = time(NULL);
	
	struct tm lt;
	localtime_r(&t1, &lt);

	sprintf(buf, "%02d:%02d:%02d", lt.tm_hour, lt.tm_min, lt.tm_sec);

	return true;
}

bool Utility::get_date_time_string(char * buf, int buf_len, time_t unix_time)
{
	if(buf == NULL || buf_len < 18)
		return false;

	time_t t1 = unix_time; 
	if(t1 == 0)
		t1 = time(NULL);
	
	struct tm lt;
	localtime_r(&t1, &lt);

	sprintf(buf, "%04d%02d%02d %02d:%02d:%02d",  (lt.tm_year + 1900), (lt.tm_mon + 1), lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);

	return true;
}


string Utility::get_date_time_string(time_t unix_time)
{
	char buf[32];
	
	time_t t1 = unix_time;
	if(0 == t1)
		t1 = time(NULL);
	
	struct tm lt;
	localtime_r(&t1, &lt);

	sprintf(buf, "%04d%02d%02d %02d:%02d:%02d",  (lt.tm_year + 1900), (lt.tm_mon + 1), lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);

	return string(buf);
}


bool Utility::set_fd_block(int fd, bool block_mode)
{
	int flag = fcntl(fd, F_GETFL);
	if(flag == -1)
	{
		return false;
	}

	if(block_mode)
		flag &= (~O_NONBLOCK);
	else
		flag |= O_NONBLOCK;

	if(fcntl(fd, F_SETFL, flag) == -1)
	{
		return false;
	}
	
	return true;
}

bool Utility::set_socket_tcpcork(int fd, bool turnon)
{
	int state = turnon ? 1 : 0;
	return 0 == setsockopt(fd, IPPROTO_TCP, TCP_CORK, &state, sizeof(state));
}

bool Utility::set_socket_linger(int fd, bool turnon, int time_out_secs)
{
	struct linger so_linger;
	so_linger.l_onoff = turnon ? 1 : 0;
	so_linger.l_linger = time_out_secs;
	return 0 == setsockopt (fd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger));
}

bool Utility::setReuseAddr(int fd)
{
	int iReuseAddrFlag=1;
	
	int iSetResult = -1;
	iSetResult = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, \
		(char*)&iReuseAddrFlag, sizeof(iReuseAddrFlag));
	if (-1 == iSetResult)
	{
		return false;
	}

	return true;
}


int Utility::recv_nonblock_data(int sock_fd, char * buffer, int & recv_len)
{
	int recv_bytes = 0;
	while(recv_bytes < recv_len)
	{
		int ret = recv(sock_fd, buffer + recv_bytes, recv_len - recv_bytes, 0);
		if(ret < 0)
		{
			int err = errno;
			if(err == EAGAIN)
			{
				recv_len = recv_bytes;
				if(recv_bytes > 0)
				{
					return recv_bytes;
				}
				else
				{
					return NONBLOCK_RECV_AGAIN;
				}
			}
			else if(err == EINTR)
			{
				continue;
			}
			else
			{
				recv_len = recv_bytes;
				return NONBLOCK_RECV_ERROR;
			}
		}
		else if(ret == 0)
		{
			// eof (possibly connection reset by peer)
			recv_len = recv_bytes;
			return 0;
		}

		recv_bytes += ret;
	}
	
	return recv_bytes;
}


int Utility::send_nonblock_data(int sock_fd, const char * buffer, int  bytes_tosend)
{
	int bytes_sent = 0;
	while(bytes_sent < bytes_tosend)
	{
		int ret = send(sock_fd, buffer + bytes_sent, bytes_tosend - bytes_sent, 0);
		if(ret < 0)
		{
			int err = errno;
			if(err == EAGAIN)
			{
				return bytes_sent;
			}
			else if(err == EINTR)
			{
				continue;
			}
			else
			{
				return -1;
			}
		}

		bytes_sent += ret;
	}
	
	return bytes_sent;
}

// receive date in block-mode
int Utility::recv_data(int sock_fd, char * buffer, int  recv_len)
{
	int recv_bytes = 0;
	while(recv_len > 0)
	{
		recv_bytes = recv(sock_fd, buffer, recv_len, 0);
		if(recv_bytes == 0)
		{
			// //LOG4CPLUS_INFO(logger, "peer close connection when recv data from socket "<<fd_sock);
			return 0;
		}
		else if(recv_bytes < 0)
		{
		       int err = errno;
			if(err == EINTR)
			{
				continue;
			}
			// //LOG4CPLUS_WARN(logger, "error:"<<errno<<" when recv data from socket "<<fd_sock);
			return -1;
		}

		buffer += recv_bytes;
		recv_len -= recv_bytes;
	}
	
	return 1;
}

// send date in block-mode
int Utility::send_data(int sock_fd, const char * buffer, int  bytes_tosend)
{
	int bytes_sent = 0;
	while(bytes_sent < bytes_tosend)
	{
		int ret = send(sock_fd, buffer + bytes_sent, bytes_tosend - bytes_sent, 0);
		if(ret < 0)
		{
			int err = errno;
			if(err == EINTR)
			{
				continue;
			}
			else
			{
				return -1;
			}
		}

		bytes_sent += ret;
	}
	
	return bytes_sent;
	
}

string Utility::get_peer_ip(int fd)
{
	struct sockaddr_in peer_addr;
	int len = sizeof(peer_addr);
	if(0 == getpeername(fd, (struct sockaddr*)&peer_addr, (socklen_t*)&len))
		return string(inet_ntoa(peer_addr.sin_addr));
	else
		return "";
}

string Utility::get_local_ip(int fd)
{
	struct sockaddr_in peer_addr;
	int len = sizeof(peer_addr);
	if(0 == getsockname(fd, (struct sockaddr*)&peer_addr, (socklen_t*)&len))
		return string(inet_ntoa(peer_addr.sin_addr));
	else
		return "";
}


_u32 Utility::get_peer_ip_int(int fd)
{
	struct sockaddr_in peer_addr;
	int len = sizeof(peer_addr);
	if(0 == getpeername(fd, (struct sockaddr*)&peer_addr, (socklen_t*)&len))
		return peer_addr.sin_addr.s_addr;
	else
		return 0;
}

_u32 Utility::get_local_ip_int(int fd)
{
	struct sockaddr_in peer_addr;
	int len = sizeof(peer_addr);
	if(0 == getsockname(fd, (struct sockaddr*)&peer_addr, (socklen_t*)&len))
		return peer_addr.sin_addr.s_addr;
	else
		return 0;
}


bool Utility::read_mac_addr(char * buf, int & buffer_len)
{
	if(buf == NULL || buffer_len < 6)
		return false;

	struct ifreq ifr;
	struct ifreq *IFR;
	struct ifconf ifc;
	char if_buf[1024];
	int ok = 0;

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd==-1) {
		return false;
	}

	ifc.ifc_len = sizeof(if_buf);
	ifc.ifc_buf = if_buf;
	ioctl(fd, SIOCGIFCONF, &ifc);

	IFR = ifc.ifc_req;
	for (int i = ifc.ifc_len / sizeof(struct ifreq); --i >= 0; IFR++) {
		strcpy(ifr.ifr_name, IFR->ifr_name);
		if (ioctl(fd, SIOCGIFFLAGS, &ifr) == 0) {
			if (! (ifr.ifr_flags & IFF_LOOPBACK)) {
				if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) {
					ok = 1;
					break;
				}
			}
		}
	}

	close(fd);
	if (ok) {
		memcpy(buf, ifr.ifr_hwaddr.sa_data, 6);
		buffer_len = 6;
	}
	else {
		return false;
	}
	
	return true;
}

string Utility::get_local_peerid(unsigned bind_addr, uint16_t listen_port, int index)
{
	char addr_buf[12];
	char suffix_buf[8];

	_u32 key = bind_addr + listen_port;
	map<_u32, string>::iterator it = _peerid_maps.find(key);
	if(it != _peerid_maps.end())
		return it->second;

	int buf_len = sizeof(addr_buf);
	if(!read_mac_addr(addr_buf, buf_len))
	{
		fprintf(stderr, "failed to read MAC address of local host.\n");
		return (string)"??@^#&$!~%??0001";
	}

	string peerid = bytes_to_hex((uint8_t*)addr_buf, buf_len);
	unsigned extra = key; // % 4096;
	snprintf(suffix_buf, sizeof(suffix_buf), "%04X", extra);
	peerid.append(suffix_buf);
	
	int paddings = 16 - peerid.length();
	if(paddings > 0)
		peerid.append(paddings, '0');

	peerid[0] += index;

	_peerid_maps[key] = peerid;

	return peerid;
}

string Utility::get_product_peerid(unsigned bind_addr, uint16_t listen_port)
{
	return get_local_peerid(bind_addr, listen_port);

/*
	char addr_buf[12];

	_u32 key = listen_port << 16 || (product_flag & 0xFFFF);
	map<_u32, string>::iterator it = _peerid_maps.find(key);
	if(it != _peerid_maps.end())
		return it->second;

	int buf_len = sizeof(addr_buf);
	if(!read_mac_addr(addr_buf, buf_len))
	{
		fprintf(stderr, "failed to read MAC address of local host.\n");
		return (string)"??@^#&$!~%??0047";
	}

	string peerid = bytes_to_hex((uint8_t*)addr_buf, buf_len);

	int16_t port = (int16_t)(listen_port % 100);
	int16_t product_suffix =(int16_t)((product_flag << 4) + 7); // 7 is magic number
	char suffix[16];
	snprintf(suffix, sizeof(suffix), "%02X%02X", (int)port, (int)product_suffix);
	peerid.append(suffix);

	_peerid_maps[key] = peerid;

	return peerid;
*/	
}


int Utility::SplitString(char *srcStr, const string &delim, vector<string>& strList)
{
	strList.clear();
	char *lasts;
	char *sToken = strtok_r(srcStr, delim.c_str(), &lasts);			
	while (sToken)
	{
		strList.push_back(sToken);
		sToken = strtok_r(NULL, delim.c_str(), &lasts);
	}
	return strList.size();
}


int Utility::split(const std::string& source, const char* delimitor, std::vector<std::string>& result_array)
{
	if(delimitor == NULL)
		return 0;

	result_array.clear();

	string::size_type startPos = 0;
	bool reachEnd = false;
	while(!reachEnd)
	{
		string::size_type curPos = source.find(delimitor, startPos);
		if(curPos != string::npos)
		{
			result_array.push_back(source.substr(startPos, curPos - startPos));
			startPos = curPos + 1;
		}
		else
		{
			// add the last part
			if(startPos < source.length())
				result_array.push_back(source.substr(startPos));
			
			reachEnd = true;
		}
	}

	return result_array.size();
}

_u32 Utility::string_to_hash(const string &str)
{
	int result = 1;
	
	int len = str.size();
	len = (len > 32 ? 32 : len);
	uint8_t *ptr = (uint8_t *)str.c_str();
	
 	for (int i = 0; i < len; i++)
		result = 31 * result + ptr[i];
 
	return abs(result);
}

_u32 Utility::get_filelast_mtime(const char *file_path)
{
	if(file_path == NULL)
	{
		return 0;
	}
	struct stat64 buf;
	if(lstat64(file_path, &buf) < 0)
	{
		return false;
	}
	return buf.st_mtime;
}

bool Utility::file_exists(const char * file_path, bool exclude_dir)
{
	if(file_path == NULL)
		return false;

	struct stat64 buf;
	if(lstat64(file_path, &buf) < 0)
		return false;

	if(exclude_dir)
		return S_ISREG(buf.st_mode);
	else
		return S_ISREG(buf.st_mode) || S_ISDIR(buf.st_mode);
}

_u64 Utility::get_filesize_by_fd(const int &fd)
{
	if(fd <= 0)
		return 0;
	struct stat64 buf;
	if(fstat64(fd, &buf) < 0)
		return 0;

	return buf.st_size;
}

bool Utility::is_valid_seedfile(const char * file_path, _u64 min_filesize)
{
	if(file_path == NULL)
		return false;

	struct stat64 buf;
	if(lstat64(file_path, &buf) < 0)
	{
		return false;
	}

	return S_ISREG(buf.st_mode) && (_u64)buf.st_size >= min_filesize;
}

bool Utility::is_valid_seedfile(const struct stat64 &buf, _u64 min_filesize)
{
	return S_ISREG(buf.st_mode) && (_u64)buf.st_size >= min_filesize;
}

bool Utility::is_directory(const char * file_path, string &realpath)
{
	if(file_path == NULL)
		return false;

	struct stat64 buf;
	if(lstat64(file_path, &buf) < 0)
		return false;
	
	if(S_ISLNK(buf.st_mode))
	{
		char path[1024] = {0};
		if(readlink(file_path, path, 1024) > 0)
		{
			struct stat64 buf2;
			if(lstat64(path, &buf2) < 0)
			{
				return false;
			}
			realpath = path;
			return S_ISDIR(buf2.st_mode);
		}
		else
		{
			return false;
		}
	}
	else
	{
		realpath = file_path;
		return S_ISDIR(buf.st_mode);
	}
}

bool Utility::is_directory(const char * file_path, const struct stat64 &buf, string &realpath)
{	
	if(S_ISLNK(buf.st_mode))
	{
		char path[1024] = {0};
		if(readlink(file_path, path, 1024) > 0)
		{
			struct stat64 buf2;
			if(lstat64(path, &buf2) < 0)
			{
				return false;
			}
			realpath = path;
			return S_ISDIR(buf2.st_mode);
		}
		else
		{
			return false;
		}
	}
	else
	{
		realpath = file_path;
		return S_ISDIR(buf.st_mode);
	}
}

_u64 Utility::retrieve_filesize(const char * file_path)
{
	if(file_path == NULL)
		return 0;

	struct stat64 buf;
	if(lstat64(file_path, &buf) < 0)
		return 0;

	return (_u64)buf.st_size;
}

bool Utility::cacl_threshold_md5_hex(int threshold, _u64 filesize, char* md5hex_buffer)
{
	static uint8_t trailing_bytes[] = {47, 13, 94, 118, 39, 71, 156, 59};
	uint8_t values[16], hashed_md5[16];

	if(md5hex_buffer == NULL)
		return false;

	threshold = ~threshold;
	memcpy(values, &threshold, sizeof(int));
	_u32 lower_size = (_u32)filesize;
	memcpy(values + sizeof(int), &lower_size, sizeof(int));
	memcpy(values + sizeof(int) * 2, trailing_bytes, sizeof(trailing_bytes));

	MD5_CTX ctx;
	MD5_Init(&ctx);	
	MD5_Update(&ctx, values, sizeof(values));	
	MD5_Final(hashed_md5, &ctx);

	for(int i = 0; i < 16; i++)
	{
		byte_to_hex(hashed_md5[i], md5hex_buffer + i * 2);
	}	
	md5hex_buffer[32] = '\0';

	return true;	
}

int Utility::connect_by_ip_port(const char * server_ip, int server_port)
{
	if(server_ip == NULL)
		return -1;

	int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock_fd == -1)
	{
		return -1;
	}	

	struct sockaddr_in addr;                                                                                       
	addr.sin_family = AF_INET;                                                                                     
	addr.sin_addr.s_addr = inet_addr(server_ip); 
	addr.sin_port = htons(server_port);      
	                                                                                                              
	if(connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)                                              
	{                                                                                                              
		close(sock_fd);
		return -1;
	}     

	return sock_fd;	
}

string Utility::ip_ntoa(uint32_t ip_num)
{
	char ip_text[20];

	uint8_t* ip = (uint8_t*)&ip_num;
	sprintf(ip_text, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

	return (string)ip_text;
}

bool Utility::ip_ntoa(uint32_t ip_num, char* buf, int buf_len)
{
	if(buf == NULL || buf_len < 16)
		return false;

	uint8_t* ip = (uint8_t*)&ip_num;
	sprintf(buf, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

	return true;
}

string Utility::itostr(__int32 num)
{
	char buf[32] = {0};
	sprintf(buf, "%d", num);
	return string(buf);
}

string Utility::itostr(_u32 num)
{
	char buf[32] = {0};
	sprintf(buf, "%u", num);
	return string(buf);
}


string Utility::itostr(__int64 num)
{
	char buf[32] = {0};
	sprintf(buf, "%lld", num);
	return string(buf);
}

string Utility::itostr(_u64 num)
{
	char buf[32] = {0};
	sprintf(buf, "%llu", num);
	return string(buf);
}


string Utility::filesize_readable_text(_u64 filesize)
{
	char size_text[32];
	if(filesize < _100_KB)
	{
		int size_bytes = (int)filesize;
		sprintf(size_text, "%d", size_bytes);
	}
	else if(filesize < _100_MB)
	{
		int size_kb = (int)(filesize / SIZE_KB);
		sprintf(size_text, "%dK", size_kb);
	}
	else if(filesize < _100_GB)
	{
		int size_mb = (int)(filesize / SIZE_MB);
		sprintf(size_text, "%dM", size_mb);		
	}
	else
	{
		int size_gb = (int)(filesize / SIZE_GB);
		sprintf(size_text, "%dG", size_gb);		
	}

	return (string)size_text;
}

string Utility::extract_filename(const char * filepath)
{
	if(filepath == NULL)
		return "";

	const char* pos = strrchr(filepath, '/');
	if(pos == NULL)
		pos = strrchr(filepath, '\\');
	if(pos == NULL)
		return (string)filepath;

	return (string)(pos + 1);	
}

string Utility::extract_filename_erase_td(const char * filepath)
{
	if(filepath == NULL)
		return "";

       if(Utility::ends_with(filepath,".td") == true)
       {
             char pathbuf[MAX_PATH_LEN];
	      memset(pathbuf,0,MAX_PATH_LEN);	 
	      strncpy(pathbuf, filepath, MAX_PATH_LEN - 1);
	      pathbuf[MAX_PATH_LEN - 1] = '\0';   
	   
             char* pos1 = strrchr(pathbuf, '.');

		*(pos1) = '\0';  // remove .td 
		
		char* pos = strrchr(pathbuf, '/');
	       if(pos == NULL)
		        pos = strrchr(pathbuf, '\\');
	      if(pos == NULL)
		       return (string)pathbuf;

	      return (string)(pos + 1);		 
       }

	const char* pos = strrchr(filepath, '/');
	if(pos == NULL)
		pos = strrchr(filepath, '\\');
	if(pos == NULL)
		return (string)filepath;

	return (string)(pos + 1);	
}

string Utility::extract_parent_dir(const char * filepath)
{
	if(filepath == NULL)  // return root dir
		return "/";  

	char pathbuf[MAX_PATH_LEN];
	strncpy(pathbuf, filepath, MAX_PATH_LEN - 1);
	pathbuf[MAX_PATH_LEN - 1] = '\0';

	int len = (int)strlen(pathbuf);
	if(pathbuf[len - 1]  == '/') // remove tailing '/' char
		pathbuf[len - 1] = '\0';  

	char* pos = strrchr(pathbuf, '/');
	if(pos == NULL)  // only filename part, so return root dir
		return "/";
	*(pos + 1) = '\0';  // remove filename part

	return (string)pathbuf;
}

bool Utility::ends_with(const char* fullstr, const char* substr)
{
	if(fullstr == NULL || substr == NULL)
		return false;

	int full_len = strlen(fullstr);
	int sub_len = strlen(substr);
	if(sub_len > full_len)
		return false;

	return 0 == memcmp(fullstr + full_len - sub_len, substr, sub_len);
}

void Utility::sleep_in_seconds(int seconds)
{
	_u64 start = current_time_ms();
	int sec_sleep = seconds;
	while(true)
	{
		int remain = sleep(sec_sleep);
		if(remain == 0)
			break;
		
		_u64 now = current_time_ms();
		sec_sleep = seconds - (now - start) / 1000;
		if(sec_sleep <= 0)
			break;
	}
}

// this implementation is faster than std c lib at most times (because do not fill trailing zeros) 
char* Utility::strncpy(char * dest, const char * src, int count)
{
	for(int i = 0; i < count; i++)
	{
		dest[i] = src[i];
		if(!src[i])
			break;
	}
	
	return dest;
}

bool Utility::set_socket_send_timeout(int fd, int timeout_sec)
{
	struct timeval time;
	time.tv_sec = timeout_sec;
	time.tv_usec = 0;

	bool succ = (0 == setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &time, sizeof(time)));
	return succ;
}

bool Utility::set_socket_recv_timeout(int fd, int timeout_sec)
{
	struct timeval time;
	time.tv_sec = timeout_sec;
	time.tv_usec = 0;

	bool succ = (0 == setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(time)));
	return succ;
}

string Utility::get_first_ip_from_domainname(const char *domainname)
{
    struct hostent hostbuf,*res = NULL;
    char buf[4048] = {0};
    int err = 0;
    gethostbyname_r(domainname, &hostbuf, buf, sizeof(buf), &res, &err);
    if(!res){
        return string("");
    }

    char **q = res->h_addr_list;
    struct in_addr in;
    memcpy(&in.s_addr, *q, sizeof(in.s_addr));
    char ip_str[32] = {0};
    inet_ntop(AF_INET, (char*)&in, ip_str, sizeof(ip_str));
    return string(ip_str);
}


_u64  Utility::get_allign_offset(_u64  offset, uint32_t  allign_base)
{
        return (offset/allign_base) * allign_base;
}

_u32  Utility::get_allign_length(_u32 length,uint32_t  allign_base)
{
         return ((length + allign_base -1)/allign_base) * allign_base;
}


/*
_u64 Utility::get_filesize_by_path(const char *filepath)
{
	if(filepath == NULL)
	{
		return 0;
	}
	struct stat64 buf;
	if(lstat64(filepath, &buf) < 0)
	{
		return 0;
	}
	return (_u64)buf.st_size;
}
*/

//\CC滻string\D6е\C4\CB\F9\D3\D0\D7ַ\FB
 _u32   Utility::replace_all(std::string& str,  const std::string& pattern,  const std::string& newpat)
{
	 _u32 count = 0; 
        const size_t nsize = newpat.size(); 
        const size_t psize = pattern.size(); 
 
        for(size_t pos = str.find(pattern, 0);  
			pos != std::string::npos; 
            pos = str.find(pattern,pos + nsize)) 
        { 
            str.replace(pos, psize, newpat); 
            count++; 
        } 
 
        return count; 
}
#if 0
//\B4\FA\C2\EBת\BB\BB:\B4\D3һ\D6ֱ\E0\C2\EBתΪ\C1\EDһ\D6ֱ\E0\C2\EB
int Utility::code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t &outlen)
{
	size_t ret;
	iconv_t cd;
	char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset,from_charset);
	if (cd==0) return -1;
	memset(outbuf,0,outlen);
	ret = iconv(cd,pin,&inlen,pout,&outlen);
	iconv_close(cd);
	return ret;
}
//UNICODE\C2\EBתΪGB2312\C2\EB
int Utility::u2g(char *inbuf,size_t inlen,char *outbuf,size_t &outlen)
{
	return code_convert("UTF-8","GBK",inbuf,inlen,outbuf,outlen);
}
//GB2312\C2\EBתΪUNICODE\C2\EB
int Utility::g2u(char *inbuf,size_t inlen,char *outbuf,size_t &outlen)
{
	return code_convert("GBK","UTF-8",inbuf,inlen,outbuf,outlen);
}
#endif
uint32_t Utility::utf8_decode( char *s, uint32_t *pi )
{
    uint32_t c;
    int i = *pi;
    /* one digit utf-8 */
    if ((s[i] & 128)== 0 ) {
        c = (uint32_t) s[i];
        i += 1;
    } else if ((s[i] & 224)== 192 ) { /* 110xxxxx & 111xxxxx == 110xxxxx */
        c = (( (uint32_t) s[i] & 31 ) << 6) +
            ( (uint32_t) s[i+1] & 63 );
        i += 2;
    } else if ((s[i] & 240)== 224 ) { /* 1110xxxx & 1111xxxx == 1110xxxx */
        c = ( ( (uint32_t) s[i] & 15 ) << 12 ) +
            ( ( (uint32_t) s[i+1] & 63 ) << 6 ) +
            ( (uint32_t) s[i+2] & 63 );
        i += 3;
    } else if ((s[i] & 248)== 240 ) { /* 11110xxx & 11111xxx == 11110xxx */
        c =  ( ( (uint32_t) s[i] & 7 ) << 18 ) +
            ( ( (uint32_t) s[i+1] & 63 ) << 12 ) +
            ( ( (uint32_t) s[i+2] & 63 ) << 6 ) +
            ( (uint32_t) s[i+3] & 63 );
        i+= 4;
    } else if ((s[i] & 252)== 248 ) { /* 111110xx & 111111xx == 111110xx */
        c = ( ( (uint32_t) s[i] & 3 ) << 24 ) +
            ( ( (uint32_t) s[i+1] & 63 ) << 18 ) +
            ( ( (uint32_t) s[i+2] & 63 ) << 12 ) +
            ( ( (uint32_t) s[i+3] & 63 ) << 6 ) +
            ( (uint32_t) s[i+4] & 63 );
        i += 5;
    } else if ((s[i] & 254)== 252 ) { /* 1111110x & 1111111x == 1111110x */
        c = ( ( (uint32_t) s[i] & 1 ) << 30 ) +
            ( ( (uint32_t) s[i+1] & 63 ) << 24 ) +
            ( ( (uint32_t) s[i+2] & 63 ) << 18 ) +
            ( ( (uint32_t) s[i+3] & 63 ) << 12 ) +
            ( ( (uint32_t) s[i+4] & 63 ) << 6 ) +
            ( (uint32_t) s[i+5] & 63 );
        i += 6;
    } else {
        c = '?';
        i++;
    }
    *pi = i;
    return c;
}

std::string Utility::UrlEncode(const std::string& src)
{
    static    char hex[] = "0123456789ABCDEF";
    std::string dst;
    
    for (size_t i = 0; i < (size_t)src.size(); i++)
    {
        uint8_t ch = src[i];
        if (isalnum(ch))
        {
            dst += ch;
        }
        else
            if (src[i] == ' ')
            {
                dst += '+';
            }
            else
            {
                uint8_t c = static_cast<uint8_t>(src[i]);
                dst += '%';
                dst += hex[c / 16];
                dst += hex[c % 16];
            }
    }
    return dst;
}

std::string Utility::UrlDecode(const std::string& src)
{
    std::string dst, dsturl;

    int srclen = src.size();

    for (int i = 0; i < srclen; i++)
    {
        if (src[i] == '%')
        {
            if(isxdigit(src[i + 1]) && isxdigit(src[i + 2]))
            {
                char c1 = src[++i];
                char c2 = src[++i];
                c1 = c1 - 48 - ((c1 >= 'A') ? 7 : 0) - ((c1 >= 'a') ? 32 : 0);
                c2 = c2 - 48 - ((c2 >= 'A') ? 7 : 0) - ((c2 >= 'a') ? 32 : 0);
                dst += (uint8_t)(c1 * 16 + c2);
            }
        }
        else
            if (src[i] == '+')
            {
                dst += ' ';
            }
            else
            {
                dst += src[i];
            }
    }

    int len = dst.size();
    
    for(uint32_t pos = 0; (int)pos < len;)
    {
        uint32_t nvalue = utf8_decode((char *)dst.c_str(), &pos);
        dsturl += (uint8_t)nvalue;
    }

    return dsturl;
}

int Utility::ExecShell(const char* sCommand)
{	
	FILE *pf = popen(sCommand, "r");
	if(pf == NULL)
	{
		//LOG4CPLUS_ERROR(logger, "popen %s error" << sCommand);
		return -1;
	}

	/*
	char szBuf[2048];
	while( fgets(szBuf, sizeof(szBuf) - 1, pf) != NULL){
		// ȥ\B5\F4β\B2\BF\BB\BB\D0\D0
		if ( szBuf[strlen(szBuf) - 1] == '\n')
			szBuf[strlen(szBuf) - 1]  = '\0';
		//LOG4CPLUS_ERROR(logger, szBuf);
	}*/
	
	pclose(pf);
	return 0;
}


bool Utility::get_httpget_param(const char *buf, map<string, string> &param_list)
{
	const char *line_end = strchr(buf, '\r');
	if(line_end == NULL)
	{
		line_end = strchr(buf, '\n');
	}
	if(line_end == NULL)
	{
		//LOG4CPLUS_WARN(logger,"can`t get_httpget_param:" << buf);
		return false;
	}
	else
	{
		const char *arg_end = strchr(buf, ' ');
		if(arg_end == NULL)
		{
			arg_end = line_end;
		}

		const char *arg_start = strchr(buf, '?');
		if(arg_start == NULL)
		{
			//LOG4CPLUS_INFO(logger,"can`t find '?' in CPingCmd:" << buf);
			return false;
		}
		arg_start++;
		const char *key_begin = NULL;
		const char *key_end = NULL;
		const char *val_begin = NULL;
		const char *val_end = NULL;
		for(const char *pos = arg_start; pos <= arg_end; pos++)
		{
			if(key_begin == NULL)
			{
				key_begin = pos;
			}
			if(*pos == '=')
			{
				key_end = pos;
				val_begin = pos+1;
			}
			else if(*pos == '&' || pos == arg_end)
			{
				val_end = pos;
				string key(key_begin, key_end - key_begin);
				string val(val_begin, val_end - val_begin);
				//LOG4CPLUS_DEBUG(logger,"get key[" << key << "]=" << val);
				param_list[key] = val;
				key_begin = NULL;
				key_end = NULL;
				val_begin = NULL;
				val_end = NULL;
			}
		}
	}
	return true;
}



