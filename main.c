#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

struct arg{
	int* a;
	int* b;
};

struct res{
	int* a;
	int* b;
};

void *thread_a(void* ptr){
	volatile struct arg* arg=ptr;
	struct res* res=malloc(sizeof(struct res));
	*(arg->a)=*(arg->a)+1;
	*(arg->b)=*(arg->b)-1;
	res->a=arg->a;
	res->b=arg->b;
	return res;
}

void *thread_b(void* ptr){
	volatile struct arg* arg=ptr;
	struct res* res=malloc(sizeof(struct res));
	*(arg->a)=*(arg->a)-1;
	*(arg->b)=*(arg->b)+1;
	res->a=arg->a;
	res->b=arg->b;
	return res;
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

	void *tmp;
	pthread_join(thr_a,&tmp);
	res=tmp;
	printf("%d %d\n",*(res->a),*(res->b));
	pthread_join(thr_b,&tmp);
	res=tmp;	
	printf("%d %d\n",*(res->a),*(res->b));
	free(res);
}
