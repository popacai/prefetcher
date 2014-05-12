

def read_result(file_name):
    file_name += ".out"
    f = open(file_name)
    lines = f.readlines()
    result = []
    for line in lines:
        result.append(float(line))

    return result

def print_result(result, new):
    print "curr_cycle     %10f, %10f" % (result[0], new[0])
    print "HitRateL1      %10f, %10f" % (result[1], new[1])
    print "HitRateL2      %10f, %10f" % (result[2], new[2])
    print "AMAT           %10f, %10f" % (result[3], new[3])
    print "avgMemQ        %10f, %10f" % (result[4], new[4])
    print "L2BW           %10f, %10f" % (result[5], new[5])
    print "memBW          %10f, %10f" % (result[6], new[6])

def analyze(trace):
    base_r = read_result("./base_line/" + trace)
    new_r  = read_result(trace)

    r = diff(base_r, new_r)
    return r

def diff(r1, r2):
    r3 = []
    for i in range(len(r1)):
        r3.append(r2[i] - r1[i])

    return r3




def main():
    test_files = ["grep.trace","g++.trace","ls.trace","plamap.trace", "testgen.trace"]
    read_result("./base_line/grep.trace")


    for trace in test_files:
        print trace
        print_result(analyze(trace), read_result(trace))
        print
        print 


if __name__ == "__main__":
    main()

