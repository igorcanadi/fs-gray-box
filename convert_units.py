import sys

if len(sys.argv) < 2:
    print "usage: %s file" % sys.argv[0]

a = open(sys.argv[1]).read().split('\n')[:-1]
a = map(lambda x: map(float, x.split(' ')), a)
a = map(lambda x: (x[0] * 512.0 / 1024, x[1] / 2400.0), a)
a = map(lambda x: " ".join(map(str, x)), a)
sys.stdout.write("\n".join(a) + "\n")
