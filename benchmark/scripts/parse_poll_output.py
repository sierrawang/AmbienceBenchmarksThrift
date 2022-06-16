import sys
import statistics

def summarize_results(call_times, return_times):
    print("Results for no buffer:")
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
    line = next(it)

    call_times = []
    return_times = []

    # Check the iterations
    line = next(it)
    assert("num_iterations" in line)
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

    summarize_results(0,call_times,return_times)

    # Output the results in spreadsheet format
    print("call_times\treturn_times")
    for i in range(len(call_times)):
        print(f'{call_times[i]}\t{return_times[i]}')

main()
