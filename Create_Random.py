import random

resultlist = random.sample(range(1, 20000), 1000)

resultlist.sort()

f = open('data.txt', 'w');
for i in resultlist:
	f.write(str(i));
	f.write('\n');
f.close()

print(resultlist)