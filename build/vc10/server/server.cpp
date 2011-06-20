// server.cpp : Defines the entry point for the console application.
//

#include <concurrency/ThreadManager.h>
#include <concurrency/PosixThreadFactory.h>
#include <protocol/TBinaryProtocol.h>
#include <server/TNonblockingServer.h>

#include <iostream>
#include <stdexcept>
#include <sstream>

#include "../gen-cpp/Calculator.h"

#ifdef WIN32
#include <tchar.h>
#endif

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace apache::thrift::concurrency;

using namespace tutorial;
using namespace shared;

using namespace boost;

#ifdef WIN32
#define snprintf _snprintf_c
#endif

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

#ifdef WIN32
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char **argv)
#endif
{
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
	return 0;
}
