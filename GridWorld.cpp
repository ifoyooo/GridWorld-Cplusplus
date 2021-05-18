#include <iostream>
#include <cstdio>
#include <cstring>
#include <assert.h>
using namespace std;

#define GRID_HEIGHT 10
#define GRID_WIDTH 10
#define GRID_AREA (GRID_HEIGHT * GRID_WIDTH)
#define ACTION_NUM 4
#define PRINTU
// #define DEBUG
enum ACTION
{
    ACTION_RIGHT = 0,
    ACTION_UP,
    ACTION_LEFT,
    ACTION_DOWN
};
const int dx[4] = {1, 0, -1, 0};
const int dy[4] = {0, -1, 0, 1};
const char directionChar[4][3] = {"->", "A", "<-", "V"};


//设置U为结束点为固定值是R。
class GridWolrd
{
protected:
    int trainEpc;
    double R[GRID_AREA][ACTION_NUM];

public:
    virtual void train(int, double) = 0;
    virtual double *getResult() = 0;
    virtual void reset() = 0;
    // virtual void plot();
};

class VI : public GridWolrd
{
private:
    double U[GRID_AREA + 1][GRID_AREA];
    double compute(int k, int y, int x, int action)
    {
        int xx = x + dx[action];
        int yy = y + dy[action];
        if (xx < 0 || xx >= GRID_WIDTH)
        {
            xx = x;
        }
        if (yy < 0 || yy >= GRID_HEIGHT)
        {
            yy = y;
        }
        int s = yy * GRID_WIDTH + xx;
        return U[k][s];
    }

public:
    VI()
    {
        trainEpc = 0;
        memset(U, 0, sizeof(U));
        memset(R, 0, sizeof(R));
        for (int i = 0; i < 4; i++)
        {
            R[7 * GRID_WIDTH + 8][i] = 10;
            R[2 * GRID_WIDTH + 7][i] = 3;
            R[4 * GRID_WIDTH + 3][i] = -5;
            R[7 * GRID_WIDTH + 3][i] = -10;
        }
        for (int i = 0; i < GRID_WIDTH; i++)
        {
            R[i][ACTION_UP] -= 0.7;
            R[i][ACTION_DOWN] -= 0.1;
            R[i][ACTION_LEFT] -= 0.1;
            R[i][ACTION_RIGHT] -= 0.1;
        }
        for (int i = 0; i < GRID_WIDTH; i++)
        {
            R[(GRID_HEIGHT - 1) * GRID_WIDTH + i][ACTION_DOWN] -= 0.7;
            R[(GRID_HEIGHT - 1) * GRID_WIDTH + i][ACTION_UP] -= 0.1;
            R[(GRID_HEIGHT - 1) * GRID_WIDTH + i][ACTION_LEFT] -= 0.1;
            R[(GRID_HEIGHT - 1) * GRID_WIDTH + i][ACTION_RIGHT] -= 0.1;
        }
        for (int i = 0; i < GRID_HEIGHT; i++)
        {
            R[i * GRID_WIDTH][ACTION_LEFT] -= 0.7;
            R[i * GRID_WIDTH][ACTION_DOWN] -= 0.1;
            R[i * GRID_WIDTH][ACTION_RIGHT] -= 0.1;
            R[i * GRID_WIDTH][ACTION_UP] -= 0.1;
        }
        for (int i = 0; i < GRID_HEIGHT; i++)
        {
            R[i * GRID_WIDTH + GRID_WIDTH - 1][ACTION_RIGHT] -= 0.7;
            R[i * GRID_WIDTH + GRID_WIDTH - 1][ACTION_DOWN] -= 0.1;
            R[i * GRID_WIDTH + GRID_WIDTH - 1][ACTION_LEFT] -= 0.1;
            R[i * GRID_WIDTH + GRID_WIDTH - 1][ACTION_UP] -= 0.1;
        }
#if defined(DEBUG)
        for (int act = 0; act < 4; act++)
        {
            printf("%d\n", act);
            for (int i = 0; i < GRID_HEIGHT; i++)
            {
                for (int j = 0; j < GRID_WIDTH; j++)
                {
                    int s = i * GRID_WIDTH + j;
                    printf("%10.2lf", R[s][act]);
                }
                cout << "\n";
            }
        }
#endif
    }
    VI(double *Reward)
    {
        trainEpc = 0;
        memcpy(R, Reward, sizeof(R));
        memset(U, 0, sizeof(U));
    }
    void train(int r = 1, double gamma = 0.9)
    {
        int k = trainEpc;
        int kmost = trainEpc + r;
        for (k = trainEpc; k < kmost; k++)
        {
            bool isBest = true;
            for (int s = 0; s < GRID_AREA; s++)
            {
                int row = s / GRID_WIDTH;
                int col = s - row * GRID_WIDTH;

                if ((row==7 && col==8) || (row==2 &&col ==7))
                {
                    U[k+1][s]=R[s][0];
                    continue;
                }
                double svalue = -100;
                for (int act = 0; act < ACTION_NUM; act++)
                {
                    double tmp = 0;
                    for (int t = 0; t < ACTION_NUM; t++)
                    {
                        if (t == act)
                        {
                            tmp += 0.7 * compute(k, row, col, t);
                        }
                        else
                        {
                            tmp += 0.1 * compute(k, row, col, t);
                        }
                    }
                    double newValue = R[s][act] + gamma * tmp;
                    if (svalue < newValue)
                    {
                        svalue = newValue;
                    }
                }
                if (U[k][s] - svalue >= 0.001 || svalue - U[k][s] >= 0.001)
                {
                    isBest = false;
                }
                U[k + 1][s] = svalue;
            }
            if (isBest)
            {
                break;
            }
            trainEpc++;
        }
    }
    void reset()
    {
        memset(U, 0, sizeof(U));
        trainEpc = 0;
    }

    double *getResult()
    {
        cout << trainEpc << endl;
        cout << "---------------------------------------------------------------------------------------------------------\n";
        for (int i = 0; i < GRID_HEIGHT; i++)
        {
            for (int j = 0; j < GRID_WIDTH; j++)
            {
                int s = i * GRID_WIDTH + j;
                printf("%10.2lf", U[trainEpc][s]);
            }
            cout << "\n";
        }
        return U[trainEpc];
    }
    double *getResult(int k)
    {
        assert(k <= trainEpc);
        cout << k << endl;
        cout << "---------------------------------------------------------------------------------------------------------\n";
        for (int i = 0; i < GRID_HEIGHT; i++)
        {
            for (int j = 0; j < GRID_WIDTH; j++)
            {
                int s = i * GRID_WIDTH + j;
                printf("%10.2lf", U[k][s]);
            }
            cout << "\n";
        }
        return U[k];
    }
    int getTrainEpc()
    {
        return trainEpc;
    }
};
class GaussSeidelVI : public GridWolrd
{
private:
    double U[GRID_AREA];
    double compute(int y, int x, int action)
    {
        int xx = x + dx[action];
        int yy = y + dy[action];
        if (xx < 0 || xx >= GRID_WIDTH)
        {
            xx = x;
        }
        if (yy < 0 || yy >= GRID_HEIGHT)
        {
            yy = y;
        }
        int s = yy * GRID_WIDTH + xx;
        return U[s];
    }

public:
    GaussSeidelVI()
    {
        trainEpc = 0;
        memset(U, 0, sizeof(U));
        memset(R, 0, sizeof(R));
        for (int i = 0; i < 4; i++)
        {
            R[7 * GRID_WIDTH + 8][i] = 10;
            R[2 * GRID_WIDTH + 7][i] = 3;
            R[4 * GRID_WIDTH + 3][i] = -5;
            R[7 * GRID_WIDTH + 3][i] = -10;
        }
        for (int i = 0; i < GRID_WIDTH; i++)
        {
            R[i][ACTION_UP] -= 0.7;
            R[i][ACTION_DOWN] -= 0.1;
            R[i][ACTION_LEFT] -= 0.1;
            R[i][ACTION_RIGHT] -= 0.1;
        }
        for (int i = 0; i < GRID_WIDTH; i++)
        {
            R[(GRID_HEIGHT - 1) * GRID_WIDTH + i][ACTION_DOWN] -= 0.7;
            R[(GRID_HEIGHT - 1) * GRID_WIDTH + i][ACTION_UP] -= 0.1;
            R[(GRID_HEIGHT - 1) * GRID_WIDTH + i][ACTION_LEFT] -= 0.1;
            R[(GRID_HEIGHT - 1) * GRID_WIDTH + i][ACTION_RIGHT] -= 0.1;
        }
        for (int i = 0; i < GRID_HEIGHT; i++)
        {
            R[i * GRID_WIDTH][ACTION_LEFT] -= 0.7;
            R[i * GRID_WIDTH][ACTION_DOWN] -= 0.1;
            R[i * GRID_WIDTH][ACTION_RIGHT] -= 0.1;
            R[i * GRID_WIDTH][ACTION_UP] -= 0.1;
        }
        for (int i = 0; i < GRID_HEIGHT; i++)
        {
            R[i * GRID_WIDTH + GRID_WIDTH - 1][ACTION_RIGHT] -= 0.7;
            R[i * GRID_WIDTH + GRID_WIDTH - 1][ACTION_DOWN] -= 0.1;
            R[i * GRID_WIDTH + GRID_WIDTH - 1][ACTION_LEFT] -= 0.1;
            R[i * GRID_WIDTH + GRID_WIDTH - 1][ACTION_UP] -= 0.1;
        }
#if defined(DEBUG)
        for (int act = 0; act < 4; act++)
        {
            printf("%d\n", act);
            for (int i = 0; i < GRID_HEIGHT; i++)
            {
                for (int j = 0; j < GRID_WIDTH; j++)
                {
                    int s = i * GRID_WIDTH + j;
                    printf("%10.2lf", R[s][act]);
                }
                cout << "\n";
            }
        }
#endif
    }
    GaussSeidelVI(double *Reward)
    {
        trainEpc = 0;
        memcpy(R, Reward, sizeof(R));
        memset(U, 0, sizeof(U));
    }
    void train(int r = 1, double gamma = 0.9)
    {
        int k = trainEpc;
        int kmost = trainEpc + r;
        for (k = trainEpc; k < kmost; k++)
        {
            bool isBest = true;
            for (int s = 0; s < GRID_AREA; s++)
            {
                int row = s / GRID_WIDTH;
                int col = s - row * GRID_WIDTH;
                
                if ((row==7 && col==8) || (row==2 &&col ==7))
                {
                    U[s]=R[s][0];
                    continue;
                }
                double svalue = -100;
                for (int act = 0; act < ACTION_NUM; act++)
                {
                    double tmp = 0;
                    for (int t = 0; t < ACTION_NUM; t++)
                    {
                        if (t == act)
                        {
                            tmp += 0.7 * compute(row, col, t);
                        }
                        else
                        {
                            tmp += 0.1 * compute(row, col, t);
                        }
                    }
                    double newValue = R[s][act] + gamma * tmp;
                    if (svalue < newValue)
                    {
                        svalue = newValue;
                    }
                }
                if (U[s] - svalue >= 0.001 || svalue - U[s] >= 0.001)
                {
                    isBest = false;
                }
                U[s] = svalue;
            }
            if (isBest)
            {
                break;
            }
            trainEpc++;
        }
    }
    void reset()
    {
        memset(U, 0, sizeof(U));
        trainEpc = 0;
    }

    double *getResult()
    {
        cout << trainEpc << endl;
        cout << "---------------------------------------------------------------------------------------------------------\n";
        for (int i = 0; i < GRID_HEIGHT; i++)
        {
            for (int j = 0; j < GRID_WIDTH; j++)
            {
                int s = i * GRID_WIDTH + j;
                printf("%10.2lf", U[s]);
            }
            cout << "\n";
        }
        return U;
    }
    double *getResult(int k)
    {
        assert(k <= trainEpc);
        cout << k << endl;
        cout << "---------------------------------------------------------------------------------------------------------\n";
        for (int i = 0; i < GRID_HEIGHT; i++)
        {
            for (int j = 0; j < GRID_WIDTH; j++)
            {
                int s = i * GRID_WIDTH + j;
                printf("%10.2lf", U[s]);
            }
            cout << "\n";
        }
        return U;
    }
    int getTrainEpc()
    {
        return trainEpc;
    }
};
class PI : public GridWolrd
{
private:
    double U[GRID_AREA + 1][GRID_AREA];
    int Pi[GRID_AREA + 1][GRID_AREA];
    int trainEpc;
    double g;
    int policyEaluation(int r, double gamma)
    {
        memset(U, 0, sizeof(U));
        for (int k = 0; k < r; k++)
        {
            for (int s = 0; s < GRID_AREA; s++)
            {
                int row = s / GRID_WIDTH;
                int col = s - row * GRID_WIDTH;
                if ((row==7 && col==8) || (row==2 &&col ==7))
                {
                    U[k+1][s]=R[s][0];
                    continue;
                }
                int action = Pi[trainEpc][s];
                double tmp = 0;
                for (int t = 0; t < ACTION_NUM; t++)
                {
                    if (t == action)
                    {
                        tmp += 0.7 * compute(k, row, col, t);
                    }
                    else
                    {
                        tmp += 0.1 * compute(k, row, col, t);
                    }
                }
                U[k + 1][s] = R[s][action] + gamma * tmp;
            }
            bool isOK = true;
            for (int s = 0; s < GRID_AREA; s++)
            {
                if (U[k + 1][s] - U[k][s] >= 0.001 || U[k][s] - U[k + 1][s] >= 0.001)
                {
                    isOK = false;
                }
            }
            if (isOK)
            {
#if defined(PRINTU)
                for (int i = 0; i < GRID_HEIGHT; i++)
                {
                    for (int j = 0; j < GRID_WIDTH; j++)
                    {
                        int s = i * GRID_WIDTH + j;
                        printf("%10.2lf", U[k][s]);
                    }
                    cout << "\n";
                }
                cout << "\n";
#endif
                return k;
            }

#if defined(DEBUG)
            if (k == 0)
            {
                for (int i = 0; i < GRID_HEIGHT; i++)
                {
                    for (int j = 0; j < GRID_WIDTH; j++)
                    {
                        int s = i * GRID_WIDTH + j;
                        printf("%10s", directionChar[Pi[trainEpc][s]]);
                    }
                    cout << "\n";
                }
                cout << endl;

                for (int i = 0; i < GRID_HEIGHT; i++)
                {
                    for (int j = 0; j < GRID_WIDTH; j++)
                    {
                        int s = i * GRID_WIDTH + j;
                        printf("%10.2lf", U[k][s]);
                    }
                    cout << "\n";
                }
                cout << endl;
            }
            if (k == 1)
            {
                for (int i = 0; i < GRID_HEIGHT; i++)
                {
                    for (int j = 0; j < GRID_WIDTH; j++)
                    {
                        int s = i * GRID_WIDTH + j;
                        printf("%10.2lf", U[k][s]);
                    }
                    cout << "\n";
                }
                cout << endl;
            }
#endif
        }
        return r;
    }
    double compute(int k, int y, int x, int action)
    {
        int xx = x + dx[action];
        int yy = y + dy[action];
        if (xx < 0 || xx >= GRID_WIDTH)
        {
            xx = x;
        }
        if (yy < 0 || yy >= GRID_HEIGHT)
        {
            yy = y;
        }
        int s = yy * GRID_WIDTH + xx;
        return U[k][s];
    }

public:
    PI()
    {
        g = 0.;
        trainEpc = 0;
        memset(U, 0, sizeof(U));
        memset(R, 0, sizeof(R));
        memset(Pi, 0, sizeof(Pi)); //set the init policy.
        for (int i = 0; i < 4; i++)
        {
            R[7 * GRID_WIDTH + 8][i] = 10;
            R[2 * GRID_WIDTH + 7][i] = 3;
            R[4 * GRID_WIDTH + 3][i] = -5;
            R[7 * GRID_WIDTH + 3][i] = -10;
        }
        for (int i = 0; i < GRID_WIDTH; i++)
        {
            R[i][ACTION_UP] -= 0.7;
            R[i][ACTION_DOWN] -= 0.1;
            R[i][ACTION_LEFT] -= 0.1;
            R[i][ACTION_RIGHT] -= 0.1;
        }
        for (int i = 0; i < GRID_WIDTH; i++)
        {
            R[(GRID_HEIGHT - 1) * GRID_WIDTH + i][ACTION_DOWN] -= 0.7;
            R[(GRID_HEIGHT - 1) * GRID_WIDTH + i][ACTION_UP] -= 0.1;
            R[(GRID_HEIGHT - 1) * GRID_WIDTH + i][ACTION_LEFT] -= 0.1;
            R[(GRID_HEIGHT - 1) * GRID_WIDTH + i][ACTION_RIGHT] -= 0.1;
        }
        for (int i = 0; i < GRID_HEIGHT; i++)
        {
            R[i * GRID_WIDTH][ACTION_LEFT] -= 0.7;
            R[i * GRID_WIDTH][ACTION_DOWN] -= 0.1;
            R[i * GRID_WIDTH][ACTION_RIGHT] -= 0.1;
            R[i * GRID_WIDTH][ACTION_UP] -= 0.1;
        }
        for (int i = 0; i < GRID_HEIGHT; i++)
        {
            R[i * GRID_WIDTH + GRID_WIDTH - 1][ACTION_RIGHT] -= 0.7;
            R[i * GRID_WIDTH + GRID_WIDTH - 1][ACTION_DOWN] -= 0.1;
            R[i * GRID_WIDTH + GRID_WIDTH - 1][ACTION_LEFT] -= 0.1;
            R[i * GRID_WIDTH + GRID_WIDTH - 1][ACTION_UP] -= 0.1;
        }
    }
    void train(int r, double gamma)
    {
        g = gamma;
        int k = trainEpc;
        int kmost = trainEpc + r;
        for (; k < kmost; k++)
        {
            bool isBest = true;
#if defined(DEBUG)
            cout << "trainEpc:" << trainEpc << endl;
#endif
            int evalEpc = policyEaluation(GRID_AREA, gamma);

#if defined(DEBUG)
            cout << "evalEpc:" << evalEpc << endl;
#endif
            for (int s = 0; s < GRID_AREA; s++)
            {
                int row = s / GRID_WIDTH;
                int col = s - row * GRID_WIDTH;
                double svalue = -100;
                int action = ACTION_RIGHT;
                for (int act = 0; act < ACTION_NUM; act++)
                {
                    double tmp = 0;
                    for (int t = 0; t < ACTION_NUM; t++)
                    {
                        if (t == act)
                        {
                            tmp += 0.7 * compute(evalEpc, row, col, t);
                        }
                        else
                        {
                            tmp += 0.1 * compute(evalEpc, row, col, t);
                        }
                    }
                    double newValue = R[s][act] + gamma * tmp;
                    if (svalue < newValue)
                    {
                        action = act;
                        svalue = newValue;
                    }
                }
                Pi[k + 1][s] = action;
                if (Pi[k][s] != action)
                {
                    isBest = false;
                }
            }
            if (isBest)
            {
                break;
            }
            trainEpc++;
        }
    }
    void reset()
    {
        memset(U, 0, sizeof(U));
        memset(Pi, 0, sizeof(Pi));
        trainEpc = 0;
    }
    double *getResult()
    {
        cout << "trainEpc" << trainEpc << endl;
        cout << "---------------------------------------------------------------------------------------------------------\n";
        for (int i = 0; i < GRID_HEIGHT; i++)
        {
            for (int j = 0; j < GRID_WIDTH; j++)
            {
                int s = i * GRID_WIDTH + j;
                printf("%10s", directionChar[Pi[trainEpc][s]]);
            }
            cout << "\n";
        }
        return U[policyEaluation(GRID_AREA, g)];
    }
};

void draw(double *data)
{
    FILE *fp = fopen("data.txt", "w");
    if (fp == NULL)
    {
        cerr << "文件夹打开失败\n";
        exit(-1);
    }
    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            int s = i * GRID_WIDTH + j;
            fprintf(fp, "%10.2lf", data[s]);
        }
        fprintf(fp, "\n");
    }
}

int main()
{
    VI vi;
    vi.train(100);
    double *data;
    //38轮收敛
    cout << vi.getTrainEpc() << endl;
    vi.getResult();
    vi.getResult(0);
    vi.getResult(1);
    vi.getResult(2);
    vi.getResult(3);
    vi.getResult(4);
    GaussSeidelVI gvi;
    gvi.train(100);
    //28轮收敛
    cout << gvi.getTrainEpc() << endl;
    data = gvi.getResult();

    draw(data);
    //仅5轮就收敛了。
    PI pi;
    pi.train(99, 0.9);
    pi.getResult();
}
