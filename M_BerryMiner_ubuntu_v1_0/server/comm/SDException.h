/********************************************************************
	created:		2005/08/15
	created:		15:8:2005   22:36
	filename: 		d:\work\newcvs\chub\impl\src\common\sdexception.h
	file path:		d:\work\newcvs\chub\impl\src\common
	file base:		sdexception
	file ext:		h
	author:			
	description:	SDException
*********************************************************************/

#ifndef SANDAI_C_EXCEPTION_H_200508151508
#define SANDAI_C_EXCEPTION_H_200508151508

#include <exception>
#include <string>

class SDException : public std::exception{

public :
	SDException(const std::string& cause) : m_cause(cause){};

	virtual ~SDException() throw() {};
	virtual const char* what() const throw() { return m_cause.c_str(); };

protected:
	std::string m_cause;
};

class SDInitException : public SDException{
public:
	SDInitException(const std::string& what) : SDException(what){};
};

class SDProtocolException : public SDException{
public:
	SDProtocolException(const std::string& what) : SDException(what){};
};

class command_buffer_underflow_exception : public SDException{
public:
	command_buffer_underflow_exception(const std::string& what) : SDException(what){};
};

class command_buffer_overflow_exception : public SDException{
public:
	command_buffer_overflow_exception(const std::string& what) : SDException(what){};
};

class invalid_data_exception : public SDException
{
public:
	invalid_data_exception(const std::string& what) : SDException(what){}
};

#endif
