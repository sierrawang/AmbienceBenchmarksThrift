all: poll poll_bench_agent poll_param_bench_agent test_poll_agent test_poll_param_agent

poll: poll.cpp
	g++ ../gen-cpp/poll.cpp poll.cpp -o poll -lthrift

poll_bench_agent: poll_bench_agent.cpp
	g++ ../gen-cpp/poll.cpp ../gen-cpp/agent_types.cpp ../gen-cpp/agent.cpp poll_bench_agent.cpp -o poll_bench_agent -lthrift

poll_param_bench_agent: poll_param_bench_agent.cpp
	g++ ../gen-cpp/poll.cpp ../gen-cpp/agent_types.cpp ../gen-cpp/agent.cpp poll_param_bench_agent.cpp -o poll_param_bench_agent -lthrift

test_poll_agent: test_poll_bench_agent.cpp
	g++ ../gen-cpp/poll.cpp ../gen-cpp/agent_types.cpp ../gen-cpp/agent.cpp test_poll_bench_agent.cpp -o test_poll_agent -lthrift

test_poll_param_agent: test_poll_param_bench_agent.cpp
	g++ ../gen-cpp/poll.cpp ../gen-cpp/agent_types.cpp ../gen-cpp/agent.cpp test_poll_param_bench_agent.cpp -o test_poll_param_agent -lthrift

clean:
	rm poll poll_bench_agent poll_param_bench_agent test_poll_agent test_poll_param_agent
