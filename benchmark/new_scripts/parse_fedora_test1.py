import statistics
import sys

def parse_section(method_name, line, it):
    # Loop through unnecessary logging
    while method_name not in line:
        line = next(it)

    # Throw away the first result
    line = next(it)

    results = []
    while method_name in line:
        results.append(float(line.split()[-1]))
        line = next(it, None)
        if not line:
            break

    return results, line

def parse(method_name, results):
    print("Results for " + method_name + ":")
    print("Mean: " + str(statistics.mean(results)))
    print("Standard Deviation: " + str(statistics.stdev(results)))
    print("Variance: " + str(statistics.variance(results)))
    print("Coefficient of Variation: " + str(statistics.stdev(results)/statistics.mean(results)))
    print()

def main():
    # Open the results file
    f = open(sys.argv[1])

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

    # Quick sanity check 
    assert(len(follow) > 0)
    assert(len(follow) == len(post_tweet))

    # Parse the results
    parse("create_user()", create_user)
    parse("follow()", follow)
    parse("post_tweet()", post_tweet)
    parse("unfollow()", unfollow)
    parse("delete_user()", delete_user)

    # Output the results in spreadsheet format
    #print("create_user\tfollow\tpost_tweet\tunfollow\tdelete_user")
    #for i in range(len(create_user)):
    #    print(f'{create_user[i]}\t{follow[i]}\t{post_tweet[i]}\t{unfollow[i]}\t{delete_user[i]}')

main()

