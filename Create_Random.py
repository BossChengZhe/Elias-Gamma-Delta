import random

# resultlist = random.sample(range(1, 800000), 60000)

# resultlist.sort()

resultlist = sorted(random.sample(range(1, 40000), 1000)) + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 
resultlist = resultlist + sorted(random.sample(range(1, 40000), 1000)) 


f = open('data.txt', 'w');
for i in resultlist:
	f.write(str(i));
	f.write('\n');
f.close()
