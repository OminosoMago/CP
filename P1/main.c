#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

struct arg{
	volatile int* a;
	volatile int* b;
};

struct res{
	int* a;
	int* b;
};

void *thread_a(void* ptr){
       	struct arg* arg=ptr;
	*(arg->a)=*(arg->a)+1;
	*(arg->b)=*(arg->b)-1;
}
void *thread_b(void* ptr){
	struct arg* arg=ptr;
	*(arg->a)=*(arg->a)-1;
	*(arg->b)=*(arg->b)+1;
}

void *thread_mon(void* ptr){
	struct arg* arg=ptr;
	printf("%d %d",*(arg->a),*(arg->b));
}

int main(){
	pthread_t thr_b;
	pthread_t thr_a;
	pthread_t thr_mon;
	struct arg* arg1=malloc(sizeof(struct arg));
	arg1->a=malloc(sizeof(int));
	arg1->b=malloc(sizeof(int));
	*(arg1->a)=0;
	*(arg1->b)=0;
	struct arg arg2={
		arg1->a,
		arg1->b
	};
	struct res* res;
	for(int i=0;i<5;i++){
		pthread_create(&thr_mon,NULL,thread_mon,arg1);
		pthread_create(&thr_a,NULL,thread_a,arg1);
		pthread_create(&thr_b,NULL,thread_b,&arg2);
		pthread_create(&thr_mon,NULL,thread_mon,arg1);
	}
	pthread_join(thr_a,NULL);
	pthread_join(thr_b,NULL);

	printf("%d %d\n",*(arg1->a),*(arg1->b));
	printf("%d %d\n",*(arg2.a),*(arg2.b));
	free(res);
}
