import random

resultlist = random.sample(range(1, 800000), 40000)

resultlist.sort()

f = open('data.txt', 'w');
for i in resultlist:
	f.write(str(i));
	f.write('\n');
f.close()

print(resultlist)