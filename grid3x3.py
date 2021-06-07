import numpy as np

class Grid:
    def __init__(self,r):
        #left,down,right,up\n
        self.dx=np.array([-1,0,1,0])
        self.dy=np.array([0,-1,0,1])
        self.rewards=np.array([[r,-1,10],[-1,-1,-1],[-1,-1,-1]])
        self.v=np.zeros((3,3))
        self.r=np.zeros((3,3,4))
        for i in range(3):
            for j in range(3):
                for k in range(4):
                    self.r[i,j,k]+=0.8*self.rewards[max(0,min(2,i+self.dx[k])),max(0,min(2,self.dy[k]))]
                    self.r[i,j,k]+=0.1*self.rewards[max(0,min(2,i+self.dx[(k+1)%4])),max(0,min(2,j+self.dy[(k+1)%4]))]
                    self.r[i,j,k]+=0.1*self.rewards[max(0,min(2,i+self.dx[(k-1)%4])),max(0,min(2,j+self.dy[(k-1)%4]))]
    def train(self,gamma):
        lastv=np.ones((3,3))
        cnt=0
        while not (np.abs(lastv-self.v)<0.001).all():
            cnt+=1
            lastv=self.v.copy()
            for i in range(3):
                for j in range(3):
                    if i==0 and j==2:
                        continue
                    mmax=-10000
                    for k in range(4):
                        newmax=self.r[i,j,k]+gamma*(0.8*self.v[max(0,min(2,i+self.dx[k])),max(0,min(2,j+self.dy[k]))]+0.1*self.v[max(0,min(2,i+self.dx[(k+1)%4])),max(0,min(2,j+self.dy[(k+1)%4]))]+0.1*self.v[max(0,min(2,i+self.dx[(k-1)%4])),max(0,min(2,j+self.dy[(k-1)%4]))])
                        if mmax<newmax:
                            mmax=newmax
                    self.v[i,j]=mmax
            # print(self.v)
            # print(lastv)
        return cnt
    def __call__(self):
        for i in range(3):
            for j in range(3):
                print(self.v[i,j],end=' ')
            print()


if __name__=="__main__":
    g100=Grid(100)
    cnt=g100.train(0.99)
    print("g100 cnt =",cnt)
    g100()
    g3=Grid(3)
    cnt=g3.train(0.99)
    print("g3 cnt=",cnt)
    g3()
    g0=Grid(3)
    cnt=g0.train(0.99)
    print("g0 cnt=",cnt)
    g0()
    gn3=Grid(-3)
    cnt=gn3.train(0.99)
    print("gn3 cnt=",cnt)
    gn3()






