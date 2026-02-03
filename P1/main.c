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

int main(){
	pthread_t thr_b;
	pthread_t thr_a;
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
	pthread_create(&thr_a,NULL,thread_a,arg1);
	pthread_create(&thr_b,NULL,thread_b,&arg2);

	pthread_join(thr_a,NULL);
	pthread_join(thr_b,NULL);

	printf("%d %d\n",*(arg1->a),*(arg1->b));
	printf("%d %d\n",*(arg2.a),*(arg2.b));
	free(res);
}
