import random

# resultlist = random.sample(range(1, 1000000), 10000)

# resultlist.sort()

resultlist = []
for i in range(100):
	resultlist += sorted(random.sample(range(1, 1000000), 1000))


f = open('data.txt', 'w');
for i in resultlist:
	f.write(str(i));
	f.write('\n');
f.close()
