// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "SharedService.h"
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::shared;

class SharedServiceHandler : virtual public SharedServiceIf {
 public:
  SharedServiceHandler() {
    // Your initialization goes here
  }

  void getStruct(SharedStruct& _return, const int32_t key) {
    // Your implementation goes here
    printf("getStruct\n");
  }

};

int main(int argc, char **argv) {
  int port = 9090;
  shared_ptr<SharedServiceHandler> handler(new SharedServiceHandler());
  shared_ptr<TProcessor> processor(new SharedServiceProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();
  return 0;
}

