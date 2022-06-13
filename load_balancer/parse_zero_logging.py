import statistics

def parse_section(method_name, line, it):
    # Throw away first result
    line = next(it)

    while method_name not in line:
        print("ERROR")
        return [], ""

    results = []
    while method_name in line:
        results.append(float(line.split()[-1]))
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
    f = open('linux_zero_logging.txt')

    # Grab the lines
    lines = f.readlines()
    it = iter(lines)
    line = next(it)

    # Parse the results
    create_user, line = parse_section("create_user", line, it)
    follow, line = parse_section("follow", line, it)
    post_tweet, line = parse_section("post_tweet", line, it)
    unfollow, line = parse_section("unfollow", line, it)
    delete_user, line = parse_section("delete_user", line, it)

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
