#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

int n, m, o, v_a[1007], d_c[1007], slots[1007], filled[1007], batches[1007], signal_company[1007], val;
double x[1007];
pthread_mutex_t produce[1007], availability[1007], slot_filled[1007], var, var1, var2, mutex;

struct p_company{
	int id;
	double probability;
};

struct v_zone{
	int id;
};

struct iiitian{
	int id;
	int round;
};

int random_func(int a, int b){
	int num = (rand() % (b-a+1)) + a;
	return num; 
}

int min(int a, int b, int c){
	if(a<=b && a<=c){
		return a;
	}
	else if(b<=a && b<=c){
		return b;
	}
	else{
		return c;
	}
}

void *company(void *inp){
	struct p_company *inputs = (struct p_company*)inp;
	while(1){
		pthread_mutex_lock(&produce[inputs->id]);//
		if(val==o){
			break;
		}
		int k = random_func(1,5);
		printf("\033[1;35mPharmaceutical company %d started preparing %d batches of vaccines which have success probability %lf\n\n", inputs->id, k, inputs->probability);
		sleep(random_func(2,5));
		batches[inputs->id] = k;
		signal_company[inputs->id]=0;
		printf("\033[0;32mPharmaceutical company %d has prepared %d batches of vaccines which have success probability %lf. Waiting for all the vaccines to be used to resume production.\n\n", inputs->id, k, inputs->probability);
		int c=0;
		while(1){
			pthread_mutex_lock(&var);
			for(int i=1; i<=m; i++){
				if(v_a[i]==0){
					v_a[i]=random_func(10, 20);
					d_c[i]=inputs->id;
					pthread_mutex_unlock(&availability[i]);//
					c++;
				}
				if(c==k)
					break;
				if(val==o)
					break;
			}
			pthread_mutex_unlock(&var);
			if(c==k)
				break;
			if(val==o)
				break;
		}
		if(val==o)
			break;
	}
}

void *vaccination_zone(void *inp){
	struct v_zone *inputs = (struct v_zone*)inp;
	while(1){
		pthread_mutex_lock(&availability[inputs->id]);//
		if(val==o)
			break;
		printf("\033[1;36mPharmaceutical Company %d has delivered %d vaccines to Vaccination zone %d, resuming vaccinations now\n\n", d_c[inputs->id], v_a[inputs->id], inputs->id);
		while(1){
			printf("\033[0;33mVaccination Zone %d entering Vaccination Phase\n\n", inputs->id);
			pthread_mutex_lock(&slot_filled[inputs->id]);
			int k = random_func(1,min(8,v_a[inputs->id],o-val));
			slots[inputs->id]=k;
			filled[inputs->id]=0;
			//vaccinate[inputs->id]=0;
			printf("\033[0;34mVaccination Zone %d is ready to vaccinate with %d slots\n\n",inputs->id,k);
			pthread_mutex_lock(&slot_filled[inputs->id]);//
			sleep(2);
			if(v_a[inputs->id]==0){
				signal_company[d_c[inputs->id]]++;
				if(signal_company[d_c[inputs->id]] == batches[d_c[inputs->id]]){
					printf("\033[0;32mAll the vaccines prepared by Pharmaceutical Company %d are emptied. Resuming production now.\n\n",d_c[inputs->id]);
					pthread_mutex_unlock(&produce[d_c[inputs->id]]);//
				}
				break;
			}
			else{
				pthread_mutex_unlock(&slot_filled[inputs->id]);
			}
			if(val==o)
				break;
		}
		if(val==o)
			break;
	}
}

void *student(void *inp){
	struct iiitian *inputs = (struct iiitian*)inp;
	sleep(random_func(1, 8));
	while(1){
		int flag1=0;
		printf("\033[0mStudent %d has arrived for his %d round of Vaccination\n\n", inputs->id, inputs->round);
		sleep(3);
		printf("\033[0;31mStudent %d is waiting to be allocated a slot on a Vaccination Zone\n\n", inputs->id);
		int a;
		int flag=0;
		while(1){
			pthread_mutex_lock(&var1);
			for(int i=1; i<=m; i++){
				if(filled[i]!=slots[i]){
					filled[i]++;
					printf("\033[0;36mStudent %d assigned a slot on the Vaccination Zone %d and waiting to be vaccinated\n\n", inputs->id, i);
					a = i;
					flag=1;
					break;
				}
			}
			pthread_mutex_unlock(&var1);
			if(flag==1)
				break;
		}

		//pthread_mutex_lock(&mutex);
		if(filled[a]==slots[a] || v_a[a]==0){
			pthread_mutex_unlock(&slot_filled[a]);
			//pthread_cond_signal(&(vaccinate[a]));
		}
		/*
		pthread_cond_wait(&(vaccinate[a]), &(mutex));
		pthread_mutex_unlock(&mutex);*/

		printf("\033[0;35mStudent %d on Vaccination Zone %d has been vaccinated which has success probability %lf\n\n", inputs->id, a, x[d_c[a]]);
		float h = (float)(rand()%100)/100;
		if(h <= x[d_c[a]]){
			printf("\033[1;32mStudent %d has tested positive for antibodies.\n\n", inputs->id);
			pthread_mutex_lock(&var2);
			val++;
			pthread_mutex_unlock(&var2);
			flag1=1;
		}

		else{
			printf("\033[1;31mStudent %d has tested negative for antibodies.\n\n", inputs->id);
			inputs->round = inputs->round + 1;
			if(inputs->round == 4){
				pthread_mutex_lock(&var2);
				val++;
				pthread_mutex_unlock(&var2);
				flag1=1;
				printf("\033[1;34mStudent %d has tested negative for antibodies for 3 times. He has been sent home\n\n", inputs->id);
			}
		}
		
		if(flag1==1)
			break;
	}
	
	if(val==o){
		for(int i=1; i<=m; i++){
			pthread_mutex_unlock(&availability[i]);
			pthread_mutex_unlock(&slot_filled[i]);
		}
		for(int i=1; i<=n; i++){
			pthread_mutex_unlock(&produce[i]);
		}
	}

	printf("\033[1;33mNumber of students done: %d\n\n",val);
}

int main(){
	int allot = 0;
	pthread_mutex_init(&var, NULL);
	pthread_mutex_init(&var1, NULL);
	pthread_mutex_init(&var2, NULL);
	scanf("%d %d %d", &n, &m, &o);
	for(int i=1;i<=n;i++){
		scanf("%lf", &x[i]);
	}
	val=0;
	printf("\n");
	printf("\033[1;31mSimulation started\n\n\033[0m");
	if(n>0 && m>0 && o>0){
		pthread_t p_c[n+1];
		for(int i=1; i<=n; i++){
			pthread_mutex_init(&produce[i], NULL);
			struct p_company *p = (struct p_company*)malloc(sizeof(struct p_company));
			p->id = i;
			p->probability = x[i];
			pthread_create(&p_c[i], NULL, company, (void*)(p));
		}
		sleep(7);
		pthread_t v_z[m+1];
		for(int i=1; i<=m; i++){
			pthread_mutex_init(&availability[i], NULL);
			struct v_zone *p = (struct v_zone*)malloc(sizeof(struct v_zone));
			p->id = i;
			pthread_create(&v_z[i], NULL, vaccination_zone, (void*)(p));
		}
		pthread_t st[o+1];
		for(int i=1; i<=o; i++){
		 	struct iiitian *p = (struct iiitian*)malloc(sizeof(struct iiitian));
		 	p->id = i;
		 	p->round = 1;
		 	pthread_create(&st[i], NULL, student, (void*)(p));
		}

		for(int i=1; i<=n; i++){
			pthread_join(p_c[i], NULL);
		}
		for(int i=1; i<=m; i++){
			pthread_join(v_z[i], NULL);
		}
		for(int i=1; i<=o; i++){
		 	pthread_join(st[i], NULL);
		}
	}
	printf("\033[1;31mSimulation finished\n\n\033[0m");
}