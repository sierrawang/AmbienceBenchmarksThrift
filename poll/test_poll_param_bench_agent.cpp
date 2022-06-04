#include <cstdint>
#include <iostream>
#include <cmath>

#include "ports.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/agent.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace agent;

void calc_values(int64_t sum, int64_t sq_sum, int64_t n) {
    double _sum = static_cast<double>(sum);
    double _sq_sum = static_cast<double>(sq_sum);
    double _n = static_cast<double>(n);
    auto mean = _sum/_n;
    auto variance = (_sq_sum - (_sum * _sum)/_n)/(_n - 1);
    auto stddev = sqrt(variance);
    
    cout << "\tResults for " << n << " iterations:" << endl;
    cout << "\tMean: " << mean << " microseconds,\n\tVariance: " << variance << ",\n\tStddev: " << stddev << endl;
}

void test_bench_agent(int port, int param) {
    std::shared_ptr<TTransport> socket(new TSocket("0.0.0.0", port));
    std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    agentClient client(protocol);
    
    auto num_iterations = 100000;
    bench_result result;

    transport->open();
    client.start(result, param);
    transport->close();

    cout << "Time to pass " << param << " bytes from a service to another service:" << endl;
    calc_values(result.call_sum, result.call_sq_sum, num_iterations);

    cout << "Time to return from a service to another service:" << endl;
    calc_values(result.return_sum, result.return_sq_sum, num_iterations);
}

int main() {
    cout << "Testing interservice latency passing a buffer:" << endl;
    //auto sizes = {512, 1024, 2048, 4096, 8192};
    auto sizes = {8192};
    for (auto size : sizes) {
    	test_bench_agent(POLL_PARAM_BENCH_AGENT_PORT, size);
    }
}
