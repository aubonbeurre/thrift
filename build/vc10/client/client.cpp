// client.cpp : Defines the entry point for the console application.
//

#include <stdio.h>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <protocol/TBinaryProtocol.h>
#include <transport/TSocket.h>
#include <transport/TTransportUtils.h>
#include <async/TEvhttpClientChannel.h>

#include <event2/event.h>
#include <event2/thread.h>

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <tr1/functional>

#include "../gen-cpp/Calculator.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::async;

using namespace tutorial;
using namespace shared;

using namespace boost;

static event_base *base;

static void _log_cb(int severity, const char *msg) {
	static const char* sev[4] = {
		"DEBUG",
		"INFO",
		"WARN",
		"ERROR"
	};
	std::cout << sev[severity] << ": " << msg << std::endl;
}

static CalculatorCobClient* new_client(TProtocolFactory* protocolFactory) {
	boost::shared_ptr<TAsyncChannel> channel(
		new TEvhttpClientChannel("localhost", "/", "127.0.0.1", 8080, base));

	return new CalculatorCobClient(channel, protocolFactory);
}

static void CalculatorAsyncHandler_calculate1(TProtocolFactory* protocolFactory, CalculatorCobClient* client) {
	try {
		int32_t quotient = client->recv_calculate();
		printf("Whoa? We can divide by zero!\n");
	} catch (InvalidOperation &io) {
		printf("InvalidOperation: %s\n", io.why.c_str());
	} 

	delete client;
	event_base_loopbreak(base);
}

static void CalculatorAsyncHandler_addReturn(TProtocolFactory* protocolFactory, CalculatorCobClient* client) {
	int sum = client->recv_add();
	printf("1+1=%d\n", sum);

	delete client;
	client = new_client(protocolFactory);

	Work work;
	work.op = Operation::DIVIDE;
	work.num1 = 1;
	work.num2 = 0;

	client->calculate(tr1::bind(CalculatorAsyncHandler_calculate1, protocolFactory, std::tr1::placeholders::_1), 1, work);
}

static void CalculatorAsyncHandler_pingReturn(TProtocolFactory* protocolFactory, CalculatorCobClient* client) {
 	client->recv_ping();
	printf("ping()\n");

	delete client;
	client = new_client(protocolFactory);

	client->add(tr1::bind(CalculatorAsyncHandler_addReturn, protocolFactory, std::tr1::placeholders::_1), 1, 1);
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

	boost::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

	if(argc >= 2 && strcmp(argv[1], "http") ==  0) {
		CalculatorCobClient* client = new_client(protocolFactory.get());
		client->ping(tr1::bind(CalculatorAsyncHandler_pingReturn, protocolFactory.get(), std::tr1::placeholders::_1));

		event_base_loop(base, 0);
	} else {
		boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
		//shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

		CalculatorClient client(protocol);

		try {
			transport->open();

			client.ping();
			printf("ping()\n");

			int32_t sum = client.add(1, 1);
			printf("1+1=%d\n", sum);

			Work work;
			work.op = Operation::DIVIDE;
			work.num1 = 1;
			work.num2 = 0;

			try {
				int32_t quotient = client.calculate(1, work);
				printf("Whoa? We can divide by zero!\n");
			} catch (InvalidOperation &io) {
				printf("InvalidOperation: %s\n", io.why.c_str());
			}

			work.op = Operation::SUBTRACT;
			work.num1 = 15;
			work.num2 = 10;
			int32_t diff = client.calculate(1, work);
			printf("15-10=%d\n", diff);

			// Note that C++ uses return by reference for complex types to avoid
			// costly copy construction
			SharedStruct ss;
			client.getStruct(ss, 1);
			printf("Check log: %s\n", ss.value.c_str());

			transport->close();
		} catch (TException &tx) {
			printf("ERROR: %s\n", tx.what());
		}
	}

	return 0;
}
