import statistics

def parse_section(method_name, line, it, clock_speed):
    results = []

    # Process all create_user data
    while method_name not in line:
        line = next(it)

    while method_name in line:
        # Get before timestamp
        assert line.split()[4] == 'a'
        time_a = int(line.split()[-1])/clock_speed

        # Get after timestamp
        line = next(it)
        assert line.split()[4] == 'b'
        time_b = int(line.split()[-1])/clock_speed

        # Append the round trip time
        assert time_b > time_a
        results.append(time_b - time_a)

        line = next(it, None)
        if not line:
            break

    return results, line

def parse(method_name, results):
    print("Results for " + method_name + ":")
    print("\tMean: " + str(statistics.fmean(results)))
    print("\tStandard Deviation: " + str(statistics.stdev(results)))
    print("\tVariance: " + str(statistics.variance(results)))
    print("\tCoefficient of Variation: " + str(statistics.stdev(results)/statistics.fmean(results)))
    print()

def main():
    # Open the results file
    f = open('course_grain_results/linux_load_balancer.txt')

    # Grab the lines
    lines = f.readlines()
    it = iter(lines)

    # Grab the clockspeed
    line = next(it)
    while "RDTSC ticks per us:" not in line:
        line = next(it)
    clock_speed = int(line.split()[-1])

    # Parse the results
    create_user, line = parse_section("create_user()", line, it, clock_speed)
    follow, line = parse_section("follow()", line, it, clock_speed)
    post_tweet, line = parse_section("post_tweet()", line, it, clock_speed)
    unfollow, line = parse_section("unfollow()", line, it, clock_speed)
    delete_user, line = parse_section("delete_user()", line, it, clock_speed)

    # Parse the results
    parse("create_user()", create_user)
    parse("follow()", follow)
    parse("post_tweet()", post_tweet)
    parse("unfollow()", unfollow)
    parse("delete_user()", delete_user)

    # Output the results in spreadsheet format
    print("create_user\tfollow\tpost_tweet\tunfollow\tdelete_user")
    for i in range(len(create_user)):
        print(f'{create_user[i]}\t{follow[i]}\t{post_tweet[i]}\t{unfollow[i]}\t{delete_user[i]}')

main()

