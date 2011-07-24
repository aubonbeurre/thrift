// server.cpp : Defines the entry point for the console application.
//

#include <concurrency/ThreadManager.h>
#include <concurrency/PosixThreadFactory.h>
#include <protocol/TBinaryProtocol.h>
#include <server/TNonblockingServer.h>
#include <async/TEvhttpServer.h>
#include <async/TAsyncBufferProcessor.h>
#include <async/TAsyncProtocolProcessor.h>

#include <event2/event.h>
#include <event2/thread.h>

#include <iostream>
#include <stdexcept>
#include <sstream>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

#include "../gen-cpp/Calculator.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::async;

using namespace tutorial;
using namespace shared;

using namespace boost;

#ifdef _WIN32
#define snprintf _snprintf_c
#endif

static event_base *base;

class CalculatorHandler: public CalculatorIf {
public:
	CalculatorHandler() {
	}

	void ping() {
		printf("ping()\n");
	}

	int32_t add(const int32_t n1, const int32_t n2) {
		printf("add(%d,%d)\n", n1, n2);
		return n1 + n2;
	}

	int32_t calculate(const int32_t logid, const Work &work) {
		printf("calculate(%d,{%d,%d,%d})\n", logid, work.op, work.num1,
			work.num2);
		int32_t val;

		switch (work.op) {
		case Operation::ADD:
			val = work.num1 + work.num2;
			break;
		case Operation::SUBTRACT:
			val = work.num1 - work.num2;
			break;
		case Operation::MULTIPLY:
			val = work.num1 * work.num2;
			break;
		case Operation::DIVIDE:
			if (work.num2 == 0) {
				InvalidOperation io;
				io.what = work.op;
				io.why = "Cannot divide by 0";
				throw io;
			}
			val = work.num1 / work.num2;
			break;
		default:
			InvalidOperation io;
			io.what = work.op;
			io.why = "Invalid Operation";
			throw io;
		}

		SharedStruct ss;
		ss.key = logid;
		char buffer[12];
		snprintf(buffer, sizeof(buffer), "%d", val);
		ss.value = buffer;

		log[logid] = ss;

		return val;
	}

	void getStruct(SharedStruct &ret, const int32_t logid) {
		printf("getStruct(%d)\n", logid);
		ret = log[logid];
	}

	void zip() {
		printf("zip()\n");
	}

protected:
	map<int32_t, SharedStruct> log;

};

class CalculatorAsyncHandler: public CalculatorCobSvIf {
 protected:
  struct RequestContext {
    std::tr1::function<void(std::vector<int32_t> const& _return)> cob;
    std::vector<int32_t> ret;
    int pending_calls;
  };
public:
	CalculatorAsyncHandler() {
	}

	void ping(std::tr1::function<void()> cob) {
		printf("ping()\n");
		cob();
	}

	void add(std::tr1::function<void(int32_t const& _return)> cob, const int32_t n1, const int32_t n2) {
		printf("add(%d,%d)\n", n1, n2);
		cob(n1 + n2);
	}

	void calculate(std::tr1::function<void(int32_t const& _return)> cob, std::tr1::function<void(::apache::thrift::TDelayedException* _throw)> exn_cob, const int32_t logid, const Work &work) {
		printf("calculate(%d,{%d,%d,%d})\n", logid, work.op, work.num1,
			work.num2);
		int32_t val;

		switch (work.op) {
		case Operation::ADD:
			val = work.num1 + work.num2;
			break;
		case Operation::SUBTRACT:
			val = work.num1 - work.num2;
			break;
		case Operation::MULTIPLY:
			val = work.num1 * work.num2;
			break;
		case Operation::DIVIDE:
			if (work.num2 == 0) {
				InvalidOperation io;
				io.what = work.op;
				io.why = "Cannot divide by 0";
				exn_cob(TDelayedException::delayException(io));
				return;
			}
			val = work.num1 / work.num2;
			break;
		default:
			InvalidOperation io;
			io.what = work.op;
			io.why = "Invalid Operation";
			exn_cob(TDelayedException::delayException(io));
			return;
		}

		SharedStruct ss;
		ss.key = logid;
		char buffer[12];
		snprintf(buffer, sizeof(buffer), "%d", val);
		ss.value = buffer;

		log[logid] = ss;

		cob(val);
	}

	void getStruct(std::tr1::function<void(SharedStruct const& _return)> cob, const int32_t logid) {
		printf("getStruct(%d)\n", logid);
		cob(log[logid]);
	}

	void zip(std::tr1::function<void()> cob) {
		printf("zip()\n");
		cob();
	}

protected:
	map<int32_t, SharedStruct> log;

};

static void _log_cb(int severity, const char *msg) {
	static const char* sev[4] = {
		"DEBUG",
		"INFO",
		"WARN",
		"ERROR"
	};
#ifdef _WIN32
	int socket_errno = WSAGetLastError();
#endif
	std::cout << sev[severity] << ": " << msg << std::endl;
}

int main(int argc, char **argv)
{
	event_config *conf = event_config_new();

#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", err);
		return 1;
	}

	evthread_use_windows_threads();

	event_config_set_flag(conf, EVENT_BASE_FLAG_STARTUP_IOCP);
#endif

	base = event_base_new_with_config(conf);
	const char ** methods = event_get_supported_methods();

	std::cout << "Version: " << event_get_version() << std::endl;
	std::cout << "Method: " << event_base_get_method(base) << std::endl;
	std::cout << "Features: 0x" << std::hex << event_base_get_features(base) << std::endl;
	std::cout << "Base: " << base << std::endl;
	while(*methods) {
		std::cout << "Method: " << *methods++ << std::endl;
	}

	event_set_log_callback(_log_cb);

	if(argc >= 2 && strcmp(argv[1], "http") == 0) {
		boost::shared_ptr<CalculatorAsyncHandler> handler(new CalculatorAsyncHandler());
		boost::shared_ptr<TAsyncProcessor> proc(new CalculatorAsyncProcessor(handler));
		boost::shared_ptr<TProtocolFactory> pfact(new TBinaryProtocolFactory());
		boost::shared_ptr<TAsyncBufferProcessor> bufproc(new TAsyncProtocolProcessor(proc, pfact));
		boost::shared_ptr<TEvhttpServer> server(new TEvhttpServer(bufproc, 8080));

		printf("Starting the HTTP server port %d...\n", 8080);
		server->serve();
		printf("done.\n");
	} else {
		boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
		boost::shared_ptr<CalculatorHandler> handler(new CalculatorHandler());
		boost::shared_ptr<TProcessor> processor(new CalculatorProcessor(handler));

		// using thread pool with maximum 15 threads to handle incoming requests
		boost::shared_ptr<ThreadManager> threadManager =
			ThreadManager::newSimpleThreadManager(15);
		boost::shared_ptr<PosixThreadFactory> threadFactory = boost::shared_ptr<
			PosixThreadFactory> (new PosixThreadFactory());
		threadManager->threadFactory(threadFactory);
		threadManager->start();

		TNonblockingServer server(processor, protocolFactory, 9090, threadManager);

		printf("Starting the server...\n");
		server.serve();
		printf("done.\n");
	}

	event_base_free(base);
	event_config_free(conf);

	return 0;
}
