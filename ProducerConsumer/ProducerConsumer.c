#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define  BUFFER_LEN        5                           /* �������ĳ��� */
#define  MAX_THREAD_NUM    100                         /* ����߳���Ŀ */
typedef struct _ThreadInfo {                           /* �߳���Ϣ�ṹ */
    char entity;                                       /* �����߻��������� */
    int ID;                                            /* �̱߳�ʾ */
    int sleeptime;                                     /* ��Ϣʱ�� (��λ:����)*/
    int num;                                           /* ������Ʒ��� */
} ThreadInfo;
int                  tot = 0;                          /* ��ǰ��Ʒ����Ŀ */
int                  in, out;                          /* ��Ʒ��������������ָ�� */
sem_t                FullSemaphore;                    /* �����������ź���(���ڻ������еĲ�Ʒ��) */
sem_t                EmptySemaphore;                   /* �������յ��ź���(���л�������) */
pthread_mutex_t      Mutex;                            /* ������������д���� */
int                  buffer[BUFFER_LEN];               /* ������ */
pthread_t            Thread[MAX_THREAD_NUM];           /* �߳����� */
ThreadInfo           Thread_Info[MAX_THREAD_NUM];      /* �߳���Ϣ���� */
void produce(ThreadInfo *s) {                                       /* ������ִ�еĺ��� */
    tot++;                                                          /* ��ǰ��Ʒ������1  */
    printf("Producer %.2d producing a number %d,",s->ID,s->num);    /* ��ӡ�����߱�ʾ����Ʒ��� */
    buffer[in] = s->num;                                            /* �������Ĳ�Ʒ���뻺���� */
    in = (in > BUFFER_LEN-2) ? 0 : in+1;                            /* �ƶ���Ʒ���뻺������ָ�� */
    printf("The total number of products is %d.\n",tot);            /* ��ӡ��ǰ��Ʒ���� */
}
void *Producer(void *p) {                                           /* �������߳� */
    _sleep(((ThreadInfo*)p)->sleeptime);                            /* ��ȡ�̵߳�����ʱ�䲢���� */
    sem_wait(&EmptySemaphore);                                      /* ���л������ź������� */
    pthread_mutex_lock(&Mutex);                                     /* ���������� */
    produce((ThreadInfo*)p);                                        /* ������������ */
    pthread_mutex_unlock(&Mutex);                                   /* ���������� */
    sem_post(&FullSemaphore);                                       /* ���������ź������� */
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
    sem_init(&FullSemaphore, 0, 0);                                 /* ��ʼ���ź��� */
    sem_init(&EmptySemaphore, 0, BUFFER_LEN);                       /* ��ʼ���ź��� */
    pthread_mutex_init(&Mutex, NULL);                               /* ��ʼ�������� */
    in = out = 0;	                                                /* ��ʼ����Ʒ��������������ָ�� */
    for(i=0; i<MAX_THREAD_NUM; i++) {                               /* ���������ߡ��������߳� */
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
    pthread_exit((void *)0);	                                    /* �ȴ����߳̽��� */
}
