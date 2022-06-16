import sys
import statistics

def parse_poll_bench_agent(it, line, clock_speed):
    call_times = []
    return_times = []

    # Loop through the beginning logs
    while "Poll bench agent" not in line:
        line = next(it)
    
    # Check the iterations
    iterations = int(line.split()[-1])

    # Throw away first call and response
    line = next(it)
    line = next(it)

    line = next(it)
    while "call_time" in line:
        call_times.append(float(line.split()[-1])/clock_speed)
        line = next(it)
        return_times.append(float(line.split()[-1])/clock_speed)
        line = next(it)

    assert(len(call_times) == len(return_times))
    assert(len(call_times) == iterations - 1)

    return call_times, return_times

def parse_poll_param_bench_agent(it, line, clock_speed):
    call_times = []
    return_times = []

    # Get the size
    while "Poll param bench agent" not in line:
        line = next(it)
    size = int(line.split()[-1])
    print("Parsing results for size " + str(size))

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
        call_times.append(float(line.split()[-1])/clock_speed)
        line = next(it)
        return_times.append(float(line.split()[-1])/clock_speed)
        line = next(it, None)
        if not line:
            break

    assert(len(call_times) == len(return_times))
    assert(len(call_times) == iterations - 1)

    return call_times, return_times

def summarize_results(buffer_size, call_times, return_times):
    print("Results for buffer size\t" + str(buffer_size) + ":")
    print("Call time Mean:\t" + str(statistics.fmean(call_times)) + 
        "\tReturn time Mean:\t" + str(statistics.fmean(return_times)))
    print("Call time Standard Deviation:\t" + str(statistics.stdev(call_times)) + 
        "\tReturn time Standard Deviation:\t" + str(statistics.stdev(return_times)))
    print("Call time Variance:\t" + str(statistics.variance(call_times)) + 
        "\tReturn time Variance:\t" + str(statistics.variance(return_times)))
    print("Call time Coefficient of Variation:\t" + str(statistics.stdev(call_times)/statistics.fmean(call_times)) +
        "\tReturn time Coefficient of Variation:\t" + str(statistics.stdev(call_times)/statistics.fmean(return_times)))
    print()

def main():
    # Prepare for processing file
    output_filename = sys.argv[1]
    f = open(output_filename)
    lines = f.readlines()
    it = iter(lines)

    # Get clock_speed
    line = next(it)
    while "RDTSC ticks per us:" not in line:
        line = next(it)
    clock_speed = int(line.split()[-1])

    call_no_data,return_no_data = parse_poll_bench_agent(it, line, clock_speed)
    call_512,return_512 = parse_poll_param_bench_agent(it, line, clock_speed)
    call_1024,return_1024 = parse_poll_param_bench_agent(it, line, clock_speed)
    call_2048,return_2048 = parse_poll_param_bench_agent(it, line, clock_speed)
    call_4096,return_4096 = parse_poll_param_bench_agent(it, line, clock_speed)
    call_8192,return_8192 = parse_poll_param_bench_agent(it, line, clock_speed)

    summarize_results(0,call_no_data,return_no_data)
    summarize_results(512,call_512,return_512)
    summarize_results(1024,call_1024,return_1024)
    summarize_results(2048,call_2048,return_2048)
    summarize_results(4096,call_4096,return_4096)
    summarize_results(8192,call_8192,return_8192)

    # Output the results in spreadsheet format
    print("call_no_data\treturn_no_data\t" + 
        "call_512\treturn_512\t" +
        "call_1024\treturn_1024\t" + 
        "call_2048\treturn_2048\t" +
        "call_4096\treturn_4096\t" +
        "call_8192\treturn_8192")
    for i in range(len(call_no_data)):
        print(f'{call_no_data[i]}\t{return_no_data[i]}\t' + 
            f'{call_512[i]}\t{return_512[i]}\t' +
            f'{call_1024[i]}\t{return_1024[i]}\t' + 
            f'{call_2048[i]}\t{return_2048[i]}\t' +
            f'{call_4096[i]}\t{return_4096[i]}\t' +
            f'{call_8192[i]}\t{return_8192[i]}')

main()
