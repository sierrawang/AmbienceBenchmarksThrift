import sys
import statistics

def parse_poll_param_bench_agent(it, line):
    call_times = []
    return_times = []

    # Get the size
    while "Poll bench agent" not in line:
        line = next(it)
    
    size = int(line.split()[-1])
    print("Parsing results for size " + str(size))

    # Check that the buffer size is the same as param that was passed in
    line = next(it)
    assert(size == int(line.split()[-1]))

    # Check the iterations
    line = next(it)
    assert("Running iterations" in line)
    iterations = int(line.split()[-1])

    # Throw away the first call and response
    line = next(it)
    line = next(it)

    # Parse values
    line = next(it)
    while "call_time" in line:
        call_times.append(float(line.split()[-1]))
        line = next(it)
        return_times.append(float(line.split()[-1]))
        line = next(it, None)
        if not line:
            break

    assert(len(call_times) == len(return_times))
    assert(len(call_times) == iterations - 1)

    return call_times, return_times

def summarize_results(buffer_size, call_times, return_times):
    print("Results for buffer size\t" + str(buffer_size) + ":")
    print("Call time Mean:\t" + str(statistics.mean(call_times)) + 
        "\tReturn time Mean:\t" + str(statistics.mean(return_times)))
    print("Call time Standard Deviation:\t" + str(statistics.stdev(call_times)) + 
        "\tReturn time Standard Deviation:\t" + str(statistics.stdev(return_times)))
    print("Call time Variance:\t" + str(statistics.variance(call_times)) + 
        "\tReturn time Variance:\t" + str(statistics.variance(return_times)))
    print("Call time Coefficient of Variation:\t" + str(statistics.stdev(call_times)/statistics.mean(call_times)) +
        "\tReturn time Coefficient of Variation:\t" + str(statistics.stdev(call_times)/statistics.mean(return_times)))
    print()

def main():
    # Prepare for processing file
    output_filename = sys.argv[1]
    f = open(output_filename)
    lines = f.readlines()
    it = iter(lines)
    line = next(it)

    call_512,return_512 = parse_poll_param_bench_agent(it, line)
    call_1024,return_1024 = parse_poll_param_bench_agent(it, line)
    call_2048,return_2048 = parse_poll_param_bench_agent(it, line)
    call_4096,return_4096 = parse_poll_param_bench_agent(it, line)
    call_8192,return_8192 = parse_poll_param_bench_agent(it, line)

    summarize_results(512,call_512,return_512)
    summarize_results(1024,call_1024,return_1024)
    summarize_results(2048,call_2048,return_2048)
    summarize_results(4096,call_4096,return_4096)
    summarize_results(8192,call_8192,return_8192)

    # Output the results in spreadsheet format
    print("call_512\treturn_512\t" +
        "call_1024\treturn_1024\t" + 
        "call_2048\treturn_2048\t" +
        "call_4096\treturn_4096\t" +
        "call_8192\treturn_8192")
    for i in range(len(call_512)):
        print(f'{call_512[i]}\t{return_512[i]}\t' +
            f'{call_1024[i]}\t{return_1024[i]}\t' + 
            f'{call_2048[i]}\t{return_2048[i]}\t' +
            f'{call_4096[i]}\t{return_4096[i]}\t' +
            f'{call_8192[i]}\t{return_8192[i]}')

main()
