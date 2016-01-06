#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define  BUFFER_LEN        5                           /* 缓冲区的长度 */
#define  MAX_THREAD_NUM    100                         /* 最大线程数目 */
typedef struct _ThreadInfo {                           /* 线程信息结构 */
    char entity;                                       /* 生产者还是消费者 */
    int ID;                                            /* 线程标示 */
    int sleeptime;                                     /* 休息时间 (单位:毫秒)*/
    int num;                                           /* 生产产品编号 */
} ThreadInfo;
int                  tot = 0;                          /* 当前产品总数目 */
int                  in, out;                          /* 产品进、出缓冲区的指针 */
sem_t                FullSemaphore;                    /* 缓冲区满的信号量(已在缓冲区中的产品数) */
sem_t                EmptySemaphore;                   /* 缓冲区空的信号量(空闲缓冲区数) */
pthread_mutex_t      Mutex;                            /* 互斥锁，用于写互斥 */
int                  buffer[BUFFER_LEN];               /* 缓冲区 */
pthread_t            Thread[MAX_THREAD_NUM];           /* 线程数组 */
ThreadInfo           Thread_Info[MAX_THREAD_NUM];      /* 线程信息数组 */
void produce(ThreadInfo *s) {                                       /* 生产者执行的函数 */
    tot++;                                                          /* 当前产品总数加1  */
    printf("Producer %.2d producing a number %d,",s->ID,s->num);    /* 打印生产者标示、产品编号 */
    buffer[in] = s->num;                                            /* 将生产的产品放入缓冲区 */
    in = (in > BUFFER_LEN-2) ? 0 : in+1;                            /* 移动产品进入缓冲区的指针 */
    printf("The total number of products is %d.\n",tot);            /* 打印当前产品总数 */
}
void *Producer(void *p) {                                           /* 生产者线程 */
    _sleep(((ThreadInfo*)p)->sleeptime);                            /* 获取线程的休眠时间并休眠 */
    sem_wait(&EmptySemaphore);                                      /* 空闲缓冲区信号量操作 */
    pthread_mutex_lock(&Mutex);                                     /* 互斥锁加锁 */
    produce((ThreadInfo*)p);                                        /* 调用生产函数 */
    pthread_mutex_unlock(&Mutex);                                   /* 互斥锁解锁 */
    sem_post(&FullSemaphore);                                       /* 满缓冲区信号量操作 */
    pthread_exit((void *)0);
}
void *Consumer(void *p) {
    _sleep(((ThreadInfo*)p)->sleeptime);
    sem_wait(&FullSemaphore);
    pthread_mutex_lock(&Mutex);
    printf("Consumer %.2d receiving %d from the buffer.\n",((ThreadInfo*)p)->ID,buffer[out]);
    out = (out > BUFFER_LEN-2) ? 0 : out+1;
    pthread_mutex_unlock(&Mutex);
    sem_post(&EmptySemaphore);
    pthread_exit((void *)0);
}
int main(int argc, char *argv[]) {
    int i;
    for(i=0; i<MAX_THREAD_NUM; i++) {
        Thread_Info[i].entity = (i&1) ? 'c' : 'p';
        Thread_Info[i].ID = i+1;
        Thread_Info[i].num = i+101;
        Thread_Info[i].sleeptime = 100;
    }
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    sem_init(&FullSemaphore, 0, 0);                                 /* 初始化信号量 */
    sem_init(&EmptySemaphore, 0, BUFFER_LEN);                       /* 初始化信号量 */
    pthread_mutex_init(&Mutex, NULL);                               /* 初始化互斥量 */
    in = out = 0;	                                                /* 初始化产品进、出缓冲区的指针 */
    for(i=0; i<MAX_THREAD_NUM; i++) {                               /* 创建生产者、消费者线程 */
        switch(Thread_Info[i].entity) {
            case 'p':
                pthread_create(&Thread[i], &attr, Producer, &Thread_Info[i]);
                break;
            case 'c':
                pthread_create(&Thread[i], &attr, Consumer, &Thread_Info[i]);
                break;
            default:
                break;
        }
        //_sleep(1);
    }
    pthread_exit((void *)0);	                                    /* 等待子线程结束 */
}
