#include <cstdint>
#include <memory>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/ThreadFactory.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/TToString.h>

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <chrono>
#include <sys/time.h>
#include <ctime>

#include "ports.h"

#include "../gen-cpp/poll.h"

using namespace poll;
using namespace apache::thrift::server;
using namespace apache::thrift::transport;
using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;

using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::system_clock;

class pollHandler : public pollIf {
    public:
    pollHandler() = default;

    int64_t fn() override {
        return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
    }

    int64_t fn_with_param(const std::vector<int64_t> & data) {
        return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
    }
};

class pollCloneFactory : virtual public pollIfFactory {
    public:
    ~pollCloneFactory() override = default;
    
    pollIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) override
    {
        std::shared_ptr<::apache::thrift::transport::TSocket> sock = std::dynamic_pointer_cast<::apache::thrift::transport::TSocket>(connInfo.transport);
        return new pollHandler;
    }

    void releaseHandler(pollIf* handler) override {
        delete handler;
    }
};

int main() {
    TThreadedServer server(
        std::make_shared<pollProcessorFactory>(std::make_shared<pollCloneFactory>()),
        std::make_shared<TServerSocket>(POLL_PORT), //port
        std::make_shared<TBufferedTransportFactory>(),
        std::make_shared<TBinaryProtocolFactory>());

    server.serve();
    return 0;
}