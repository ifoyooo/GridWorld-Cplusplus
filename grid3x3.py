import numpy as np
import argparse

from numpy.core.fromnumeric import argmax

__author__="ifoyooo"
__email__="wangfuyun_000@foxmail.com"

class Grid:
    def __init__(self, r, g):
        self.dx= np.array([-1, 0, 1, 0])
        self.dy = np.array([0, -1, 0, 1])
        self.draw = ['u', 'l', 'd', 'r']
        self.rewards = np.array([[r, -1, 10], [-1, -1, -1], [-1, -1, -1]])
        self.v = np.zeros((3, 3))
        self.r = np.zeros((3, 3, 4))
        self.gamma = g
        for i in range(3):
            for j in range(3):
                for k in range(4):
                    self.r[i, j, k] += 0.8 * \
                        self.rewards[max(0, min(2, i+self.dx[k])),
                                     max(0, min(2, j+self.dy[k]))]
                    self.r[i, j, k] += 0.1*self.rewards[max(
                        0, min(2, i+self.dx[(k+1) % 4])), max(0, min(2, j+self.dy[(k+1) % 4]))]
                    self.r[i, j, k] += 0.1*self.rewards[max(
                        0, min(2, i+self.dx[(k-1) % 4])), max(0, min(2, j+self.dy[(k-1) % 4]))]

    def train(self, epsilon):
        lastv = np.ones((3, 3))
        cnt = 0
        while not (np.abs(lastv-self.v) < epsilon).all():
            cnt += 1
            lastv = self.v.copy()
            for i in range(3):
                for j in range(3):
                    if i == 0 and j == 2:
                        continue
                    mmax = -np.infty
                    for k in range(4):
                        newmax = self.r[i, j, k]+self.gamma*(0.8*self.v[max(0, min(2, i+self.dx[k])), max(0, min(2, j+self.dy[k]))]+0.1*self.v[max(0, min(2, i+self.dx[(
                            k+1) % 4])), max(0, min(2, j+self.dy[(k+1) % 4]))]+0.1*self.v[max(0, min(2, i+self.dx[(k-1) % 4])), max(0, min(2, j+self.dy[(k-1) % 4]))])
                        if mmax < newmax:
                            mmax = newmax
                    self.v[i, j] = mmax
        return cnt

    def __call__(self):
        for i in range(3):
            for j in range(3):
                print(self.v[i, j], end=' ')
            print()
        ans = np.zeros((3, 3, 4))
        for i in range(3):
            for j in range(3):
                for k in range(4):
                    ans[i, j, k] = self.r[i, j, k]+self.gamma*(0.8*self.v[max(0, min(2, i+self.dx[k])), max(0, min(2, j+self.dy[k]))]+0.1*self.v[max(0, min(2, i+self.dx[(
                        k+1) % 4])), max(0, min(2, j+self.dy[(k+1) % 4]))]+0.1*self.v[max(0, min(2, i+self.dx[(k-1) % 4])), max(0, min(2, j+self.dy[(k-1) % 4]))])
        for i in range(3):
            for j in range(3):
                if i == 0 and j == 2:
                    print("-", end=' ')
                    continue
                print(self.draw[np.argmax(ans[i, j])], end=' ')
            print()


def test1():
    g100 = Grid(100, 0.99)
    cnt = g100.train(0.001)
    print("g100 cnt =", cnt)
    g100()
    g3 = Grid(3, 0.99)
    cnt = g3.train(0.001)
    print("g3 cnt =", cnt)
    g3()
    g0 = Grid(0, 0.99)
    cnt = g0.train(0.001)
    print("g0 cnt =", cnt)
    g0()
    gn3 = Grid(-3, 0.99)
    cnt = gn3.train(0.001)
    print("gn3 cnt =", cnt)
    gn3()

def test2():
    parser = argparse.ArgumentParser()
    parser.add_argument("--r",help="rewards",nargs="+",type=float,default=[100,3,0,-3])
    parser.add_argument("--gamma",help="gamma",nargs="+",type=float,default=[0.99]*4)
    parser.add_argument("--epsilon",help="epsilon",type=int,default=0.001)
    args=parser.parse_args()
    assert len(args.r)==len(args.gamma)
    for i in range(len(args.r)):
        g=Grid(args.r[i],args.gamma[i])
        cnt=g.train(args.epsilon)
        print("grid3x3: reward =",args.r[i]," gamma =",args.gamma[i],"epsilon =",args.epsilon)
        g()


if __name__ == "__main__":
    test2()