//
// SesssionHandle.cpp
//
// $Id: //poco/1.4/Data/MySQL/src/SessionHandle.cpp#2 $
//
// Library: Data/MySQL
// Package: MySQL
// Module:  SessionHandle
//
// Copyright (c) 2008, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/Data/MySQL/SessionHandle.h"
#include "Poco/SingletonHolder.h"
#include <cstring>
#ifdef POCO_OS_FAMILY_UNIX
#include <pthread.h>
#endif


namespace Poco {
namespace Data {
namespace MySQL {


#ifdef POCO_OS_FAMILY_UNIX
class ThreadCleanupHelper
{
public:
	ThreadCleanupHelper()
	{
		if (pthread_key_create(&_key, &ThreadCleanupHelper::cleanup) != 0)
			throw Poco::SystemException("cannot create TLS key for mysql cleanup");
	}
	
	void init()
	{
		if (pthread_setspecific(_key, reinterpret_cast<void*>(1)))
			throw Poco::SystemException("cannot set TLS key for mysql cleanup");
	}
	
	static ThreadCleanupHelper& instance()
	{
		return *_sh.get();
	}
	
	static void cleanup(void* data)
	{
		mysql_thread_end();
	}
	
private:
	pthread_key_t _key;
	static Poco::SingletonHolder<ThreadCleanupHelper> _sh;
};


Poco::SingletonHolder<ThreadCleanupHelper> ThreadCleanupHelper::_sh;
#endif


SessionHandle::SessionHandle(MYSQL* mysql)
{
	h = mysql_init(mysql);

	if (!h)
	{
		throw ConnectionException("mysql_init error");
	}
#ifdef POCO_OS_FAMILY_UNIX
	ThreadCleanupHelper::instance().init();
#endif
}


SessionHandle::~SessionHandle()
{
	close();
}


void SessionHandle::options(mysql_option opt)
{
	int res = mysql_options(h, opt, 0);

	if (res != 0)
	{
		throw ConnectionException("mysql_options error", h);
	}
}


void SessionHandle::options(mysql_option opt, bool b)
{
	my_bool tmp = b;
	int res = mysql_options(h, opt, &tmp);

	if (res != 0)
	{
		throw ConnectionException("mysql_options error", h);
	}
}


void SessionHandle::connect(const char* host, const char* user, const char* password, const char* db, unsigned int port)
{
	if (!mysql_real_connect(h, host, user, password, db, port, 0, 0))
	{
		throw ConnectionException("create session: mysql_real_connect error", h);
	}
}


void SessionHandle::close()
{
	if (h)
	{
		mysql_close(h);
		h = 0;
	}
}


void SessionHandle::query(const char* str)
{
	int res = mysql_real_query(h, str, static_cast<unsigned long>(std::strlen(str))); 

	if (res != 0)
	{
		std::string msg;
		msg += "mysql_real_query('";
		msg += str;
		msg += "') error";
		throw ConnectionException(msg, h);
	}
}


} } } // namespace Poco::Data::MySQL
