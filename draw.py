import seaborn as sns
import matplotlib.pyplot as plt

fp=open("data.txt","r")
data=list(map(lambda x:list(map(float,x.split())),fp.readlines()))
print(data)
sns.heatmap(data,cmap='YlGnBu')
# plt.show()
plt.savefig("ValueIteration.jpeg")