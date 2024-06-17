#define _CRT_SECURE_NO_WARNINGS
/*
* Creator: 2022210226-肖泽宇
* K-Means Algorithm 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "BitMapPicture.h"


#define N 15
#define K 3

typedef struct
{
    float x;
    float y;
    float z;
}Point;

int center[N];  //判断每个点属于哪个簇

Point point[N] = {
 {50.0, 50.0,9.0},
 {28.0, 9.0 ,4.0},
 {17.0, 15.0,3.0},
 {25.0, 40.0,5.0},
 {28.0, 40.0,2.0},
 {50.0, 50.0,1.0},
 {50.0, 40.0,9.0},
 {50.0, 40.0,9.0},
 {40.0, 40.0,5.0},
 {50.0, 50.0,9.0},
 {50.0, 50.0,5.0},
 {50.0, 50.0,9.0},
 {40.0, 40.0,9.0},
 {40.0, 32.0,17.0},
 {50.0, 50.0,9.0},
};

char Teams[][18] = { "中国","日本","韩国","伊朗",
        "沙特","伊拉克","卡塔尔","阿联酋","乌兹别克斯坦",
        "泰国","越南","阿曼","巴林","朝鲜","印尼" };

Point mean[K];  //  store center point of each cluster


void Normalization()
{
    Point max;
    Point min;
    int i;

    min.x = min.y = min.z = 99999;
    max.x = max.y = max.z = 0;
    for (i = 0; i < N; i++)
    {
        if (max.x < point[i].x)
            max.x = point[i].x;
        if (max.y < point[i].y)
            max.y = point[i].y;
        if (max.z < point[i].z)
            max.z = point[i].z;
        if (min.x > point[i].x)
            min.x = point[i].x;
        if (min.y > point[i].y)
            min.y = point[i].y;
        if (min.z > point[i].z)
            min.z = point[i].z;
    }//find max and min of each line
 //printf("%24.0f%18.0f%18.0f\n",max.x,max.y,max.z);
 //printf("%24.0f%18.0f%18.0f\n",min.x,min.y,min.z);

    for (i = 0; i < N; i++)
    {
        point[i].x = (point[i].x - min.x) / (max.x - min.x);
        point[i].y = (point[i].y - min.y) / (max.y - min.y);
        point[i].z = (point[i].z - min.z) / (max.z - min.z);
        printf("%12.2f%18.2f%18.2f\n", point[i].x, point[i].y, point[i].z);
    }
}

//Euclidean Distance
float getDistance(Point point1, Point point2)
{
    float d;
    d = sqrt((point1.x - point2.x) * (point1.x - point2.x) 
        + (point1.y - point2.y) * (point1.y - point2.y)
        + (point1.z - point2.z) * (point1.z - point2.z));
    return d;
}

//get cluster center
void getMean(int center[N])
{
    Point tep;
    int i, j, count = 0;
    for (i = 0; i < K; ++i)
    {
        count = 0;
        tep.x = 0.0;
        tep.y = 0.0;
        tep.z = 0.0;
        for (j = 0; j < N; ++j)
        {
            if (i == center[j])
            {
                count++;
                tep.x += point[j].x;
                tep.y += point[j].y;
                tep.z += point[j].z;
            }
        }
        tep.x /= count;
        tep.y /= count;
        tep.z /= count;
        mean[i] = tep;
    }
    for (i = 0; i < K; ++i)
    {
        printf("The new center point of %d is : \t( %f, %f ,%f)\n", i + 1, mean[i].x, mean[i].y, mean[i].z);
    }
}

//Calculate SSE (Sum of square error)
float getE()
{
    int i, j;
    float cnt = 0.0, sum = 0.0;
    for (i = 0; i < K; ++i)
    {
        for (j = 0; j < N; ++j)
        {
            if (i == center[j])
            {
                cnt = (point[j].x - mean[i].x) * (point[j].x - mean[i].x) + (point[j].y - mean[i].y) * (point[j].y - mean[i].y) + (point[j].z - mean[i].z) * (point[j].z - mean[i].z);
                sum += cnt;
            }
        }
    }
    return sum;
}


void cluster()
{
    int i, j, q;
    float min;
    float distance[N][K];
    
    for (i = 0; i < N; ++i)
    {
        min = 999999.0;
        for (j = 0; j < K; ++j){
            distance[i][j] = getDistance(point[i], mean[j]);
        }
        for (q = 0; q < K; ++q){

            if (distance[i][q] < min){
                min = distance[i][q];
                center[i] = q;
            }

        }

        unsigned int Rank = 0;
        switch (center[i]+1)
        {
        case 1:Rank = 1; break;
        case 2:Rank = 2; break;
        case 3:Rank = 3; break;
        default:
            break;
        }
        printf("\t%-14s world rank %d\n", Teams[i],Rank);

    }
    printf("-----------------------------------------\n\n");
}


int main()
{
    int i, n = 0;
    float temp1;
    float temp2;
    printf("\n----------------RAW Data----------------\n");
    printf("\n\033[47;30m%14s%18s%18s%18s%\033[0m\n","Teams","2006 World Cup","2010 World Cup", "2007 Asian Cup");
    for (i = 0; i < N; ++i)
    {
        printf("%14s%12.0f%18.0f%18.0f\n",Teams[i] ,point[i].x, point[i].y, point[i].z);
    }
    printf("\n--------------Normalization--------------\n"); 
    Normalization();
    mean[0].x = point[1].x;      //center point 日本，巴林和泰国
    mean[0].y = point[1].y;
    mean[0].z = point[1].z;
    mean[1].x = point[12].x;
    mean[1].y = point[12].y;
    mean[1].z = point[12].z;
    mean[2].x = point[9].x;
    mean[2].y = point[9].y;
    mean[2].z = point[9].z;
    cluster();  //run cluster algerithm
    temp1 = getE();
    n++;

    printf("The E1 is: %f\n\n", temp1);

    getMean(center);
    cluster();  //run algerithm
    temp2 = getE(); //chect SSE
    n++;

    printf("The E2 is: %f\n\n", temp2);

    while (fabs(temp2 - temp1) != 0)//if SSE equals, then complete iterate
    {
        temp1 = temp2;
        getMean(center);
        cluster();
        temp2 = getE();
        n++;
        printf("The E%d is: %f\n", n, temp2);
    }

    printf("The total number of cluster is: %d\n\n", n);  // 统计出迭代次数
    return 0;
}
