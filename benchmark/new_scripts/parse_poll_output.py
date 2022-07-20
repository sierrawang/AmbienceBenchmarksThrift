import statistics
import sys

def parse_section(bsize, it, line):
    # Loop through unnecessary logging
    while "bench agent," not in line:
        line = next(it)

    assert((str(bsize) in line) or ("Poll bench agent" in line))
    line = next(it)
    if "data.size" in line:
        line = next(it)
        line = next(it)

    # Throw away the first results
    line = next(it)
    line = next(it)

    call_times = []
    return_times = []
    while "call_time" in line:
        assert("call_time" in line)
        call_times.append(float(line.split()[-1]))

        line = next(it)
        assert("return_time" in line)
        return_times.append(float(line.split()[-1]))

        line = next(it, None)
        if not line:
            break

    return call_times, return_times, line, it

def print_parsed(r, bsize, results):
    print(len(results))
    print(r + " for " + str(bsize) + ":")
    print("Mean: " + str(statistics.mean(results)))
    print("Standard Deviation: " + str(statistics.stdev(results)))
    print("Variance: " + str(statistics.variance(results)))
    print("Coefficient of Variation: " + str(statistics.stdev(results)/statistics.mean(results)))
    print()

def main():
    # Open the results file
    results0 = open(sys.argv[1])
    results_buf = open(sys.argv[2])

    # Grab the lines
    lines = results0.readlines() + results_buf.readlines()
    it = iter(lines)
    line = next(it)

    buffer_sizes = [0, 512, 1024, 2048, 4096, 8192]
    line = ""
    for i in range(6):
        call_times, return_times, line, it = parse_section(buffer_sizes[i], it, line)
        print_parsed("Call times", buffer_sizes[i], call_times)
        print_parsed("Return times", buffer_sizes[i], return_times)
main()

