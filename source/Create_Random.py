import random


# resultlist = random.sample(range(1, 10000000), 1000000)

# resultlist.sort()

resultlist = []
for i in range(10):
	resultlist += sorted(random.sample(range(1, 1000000), 1000))


f = open('F:\\Elias-gamma-delta\\source\\data.txt', 'w');
for i in resultlist:
	f.write(str(i));
	f.write('\n');
f.close()
