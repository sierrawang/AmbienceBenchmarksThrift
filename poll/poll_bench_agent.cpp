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
#include <chrono>
#include <sys/time.h>
#include <ctime>
#include "ports.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

#include "../gen-cpp/agent.h"
#include "../gen-cpp/poll.h"

using namespace agent;
using namespace poll;
using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::system_clock;

class agentHandler : public agentIf {
    public:
    agentHandler() = default;

    void start(bench_result& _return, const int64_t param) override {
        std::shared_ptr<TTransport> socket(new TSocket("localhost", POLL_PORT));
        std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
        std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        pollClient client(protocol);

        cout << "Poll bench agent, num_iterations: " << param << endl;
        
        auto i = 0;
        uint64_t begin = 0;
        uint64_t end = 0;
        uint64_t before = 0;
        uint64_t after = 0;
        uint64_t stamp = 0;

        // We want to calculate how long it takes to call and return from a service
        uint64_t call_time = 0;
        uint64_t call_sum = 0;
        uint64_t call_sq_sum = 0;
        uint64_t return_time = 0;
        uint64_t return_sum = 0;
        uint64_t return_sq_sum = 0;

        transport->open();
        begin = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
        for (i = 0; i < param; i++) {
            // Call another service that simply returns a timestamp
            before = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
            stamp = client.fn();
            after = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
            
            // Update the call time variables
            call_time = stamp - before;
            call_sum += call_time;
            call_sq_sum += call_time * call_time;

            // Update the return time variables
            return_time = after - stamp;
            return_sum += return_time;
            return_sq_sum += return_time * return_time;
        }
        end = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
        transport->close();

        auto total = end - begin;
        cout << total << " " << call_sum << " " << call_sq_sum <<
            " " << return_sum << " " << return_sq_sum << endl;

        _return.call_sum = call_sum;
        _return.call_sq_sum = call_sq_sum;
        _return.return_sum = return_sum;
        _return.return_sq_sum = return_sq_sum;
    }
};

class agentCloneFactory : virtual public agentIfFactory {
    public:
    ~agentCloneFactory() override = default;
    
    agentIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) override {
        std::shared_ptr<TSocket> sock = std::dynamic_pointer_cast<TSocket>(connInfo.transport);
        return new agentHandler;
    }

    void releaseHandler(agentIf* handler) override {
        delete handler;
    }
};

int main() {
    TThreadedServer server(
        std::make_shared<agentProcessorFactory>(std::make_shared<agentCloneFactory>()),
        std::make_shared<TServerSocket>(POLL_BENCH_AGENT_PORT), //port
        std::make_shared<TBufferedTransportFactory>(),
        std::make_shared<TBinaryProtocolFactory>());

    server.serve();
    return 0;
}