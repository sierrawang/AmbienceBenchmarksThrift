import sys
import statistics

def main():
    f = open(sys.argv[1])
    lines = f.readlines()
    it = iter(lines)
    line = next(it)

    # Throw away first result
    line = next(it)

    m = {}
    while True:
        toks = line.split()

        # Parse the data
        if int(toks[0]) not in m:
            m[int(toks[0])] = []

        m[int(toks[0])].append(float(toks[1]))

        # Get the next line
        line = next(it, None)
        if not line:
            break

    for num_clients in sorted(m.keys()):
        results = m[num_clients]
        print(f'{str(num_clients)}\t{str(statistics.mean(results))}\t' + 
            f'{str(statistics.stdev(results))}\t' + 
            f'{str(statistics.variance(results))}\t' + 
            f'{str(statistics.stdev(results)/statistics.mean(results))}')

main()
    
