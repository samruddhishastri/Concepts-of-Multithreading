#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<unistd.h> 
#include<stdlib.h>
#include<semaphore.h>
#include<time.h>

int k, a, e, c, t1, t2, t, astc, elec, astc_singer, elec_singer;
sem_t acoustic, electric, tshirt, stage, singer;
int a_stage[1007], e_stage[1007];
pthread_mutex_t acoustic_stage, electric_stage, as[1007], es[1007]; 
char acoustic_m_name[1007][50],electric_m_name[1007][50], acoustic_m_inst[1007][50], electric_m_inst[1007][50];

struct musician{
	char name[50];
	char inst[50];
	int time;
};

int random_func(int a, int b){
	int num = (rand() % (b - a + 1)) + a;
	return num; 
}

void *only_a(void *inp){
	struct timespec ts;
	struct musician *inputs = (struct musician*)inp;
	sleep(inputs->time);
	printf("\033[1;37m%s %s arrived\n\n\033[0m",inputs->name,inputs->inst);
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += t;
	int x = sem_timedwait(&acoustic, &ts);//wait till an acoustic stage is allocated or waiting time exceeds t
	if(x==-1){// if the waiting time exceeds t, then leave srujana
		printf("\033[1;32m%s %s left because of impatience\n\n\033[0m",inputs->name, inputs->inst);
	}
	else{
		sem_wait(&stage);//if an acoustic stage is allocated, we also need to increment the stage semaphore
		int l;
		int k = random_func(t1,t2);
		pthread_mutex_lock(&acoustic_stage);//to ensure that two musicians do not claim the same stage simultaneously
		//check which stage is free
		for(int i=1; i<=a; i++){
			if(a_stage[i]==0){//check if any performer is performing of the stage
				a_stage[i]++;//increment the individual stage performer counter
				pthread_mutex_unlock(&acoustic_stage);//unlock the acoustic stage mutex
				l = i;
				// printf("%d %d\n\n", l, i);
				strcpy(acoustic_m_name[l], inputs->name);
				strcpy(acoustic_m_inst[l], inputs->inst);
				astc--;//decrement the number acoustic stages available
				break;
			}
		}
		printf("\033[1;35m%s performing %s at acoustic stage %d for %d sec\n\n\033[0m", inputs->name, inputs->inst, l, k);
		pthread_mutex_lock(&as[l]);
		sleep(k);
		if(a_stage[l]==2){ //if a singer joins the musician during his performance wait for 2 sec
			sleep(2);
		}
		pthread_mutex_unlock(&as[l]);
		printf("\033[1;34m%s's performance at acoustic stage %d finished\n\n\033[0m",inputs->name, l);
		sem_post(&acoustic);//acoustic stage deallocated
		sem_post(&stage);//if an acoustic stage is deallocated, we also need to decrement the stage semaphore
		astc++;//increment the number acoustic stages available
		a_stage[l]=0;//decrement the individual stage performer counter
		sem_wait(&tshirt);//wait for coordinator to be free to get the tshirt
		printf("\033[1;36m%s collecting t-shirt\n\n\033[0m",inputs->name);
		sleep(2);
		sem_post(&tshirt);//leave the cooridnator so that he can attend other musicians
	}
}

void *only_e(void *inp){
	struct timespec ts;
	struct musician *inputs = (struct musician*)inp;
	sleep(inputs->time);
	printf("\033[1;37m%s %s arrived\n\n\033[0m",inputs->name,inputs->inst);
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += t;
	int x = sem_timedwait(&electric, &ts);//wait till an electric stage is allocated or waiting time exceeds t
	if(x==-1){// if the waiting time exceeds t, then leave srujana
		printf("\033[1;32m%s %s left because of impatience\n\n\033[0m",inputs->name, inputs->inst);
	}
	else{
		sem_wait(&stage);//if an electric stage is allocated, we also need to increment the stage semaphore
		int l;
		int k = random_func(t1,t2);
		pthread_mutex_lock(&electric_stage);//to ensure that two musicians do not claim the same stage simultaneously
		//check which stage is free
		for(int i=1; i<=e; i++){
			if(e_stage[i]==0){//check if any performer is performing of the stage
				e_stage[i]++;//increment the individual stage performer counter
				pthread_mutex_unlock(&electric_stage);//unlock the electric stage mutex
				l = i;
				// printf("%d %d\n\n", l, i);
				strcpy(electric_m_name[l], inputs->name);
				strcpy(electric_m_inst[l], inputs->inst);
				elec--;//decrement the number acoustic stages available
				break;
			}
		}
		printf("\033[1;35m%s performing %s at electric stage %d for %d sec\n\n\033[0m", inputs->name, inputs->inst, l, k);
		pthread_mutex_lock(&es[l]);
		sleep(k);
		if(e_stage[l]==2){//if a singer joins the musician during his performance wait for 2 sec
			sleep(2);
		}
		pthread_mutex_unlock(&es[l]);
		printf("\033[1;34m%s's performance at electric stage %d finished\n\n\033[0m",inputs->name, l);
		sem_post(&electric);//electric stage deallocated
		sem_post(&stage);//if an electric stage is deallocated, we also need to decrement the stage semaphore
		elec++;//increment the number electric stages available
		e_stage[l]=0;//decrement the individual stage performer counter
		sem_wait(&tshirt);//wait for coordinator to be free to get the tshirt
		printf("\033[1;36m%s collecting t-shirt\n\n\033[0m",inputs->name);
		sleep(2);
		sem_post(&tshirt);//leave the cooridnator so that he can attend other musicians
	}
}

void *both(void *inp){
	struct timespec ts;
	struct musician *inputs = (struct musician*)inp;
	sleep(inputs->time);
	printf("\033[1;37m%s %s arrived\n\n\033[0m",inputs->name,inputs->inst);
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += t;
	int x = sem_timedwait(&stage, &ts);//wait till a stage is allocated or waiting time exceeds t
	if(x==-1){// if the waiting time exceeds t, then leave srujana
		printf("\033[1;32m%s %s left because of impatience\n\n\033[0m",inputs->name, inputs->inst);
	}
	else{
		char stg[50];
		int flag=0;
		if(astc > 0 && elec > 0){// if both electric and acoustic stages are available
			int h = rand()%2;
			if(h==0){// acoustic stage chosen
				strcpy(stg, "acoustic");
				sem_wait(&acoustic);//increment the acoustic semaphore
				flag=1;
				astc--;//decrement the number of acoustic stages available
			}
			else{//electric stage chosen
				strcpy(stg, "electric");
				sem_wait(&electric);//increment the electric semaphore
				flag=2;
				elec--;//decrement the number of electric stages available
			}
		}
		else if(astc==0){//if only electric stages are available
			strcpy(stg, "electric");
			sem_wait(&electric);//increment the electric semaphore
			flag=2;
			elec--;//decrement the number of electric stages available
		}
		else if(elec==0){// if only acoustic stages are available
			strcpy(stg, "acoustic");
			sem_wait(&acoustic);//increment the acoustic semaphore
			flag=1;
			astc--;//decrement the number of acoustic stages available
		}
		int l;
		if(flag==1){
			pthread_mutex_lock(&acoustic_stage);//to ensure that two musicians do not claim the same stage simultaneously
			//check which stage is free
			for(int i=1; i<=a; i++){
				if(a_stage[i]==0){//check if any performer is performing of the stage
					a_stage[i]++;//increment the individual stage performer counter
					pthread_mutex_unlock(&acoustic_stage);//unlock the acoustic stage mutex
					l = i;
					strcpy(acoustic_m_name[l], inputs->name);
					strcpy(acoustic_m_inst[l], inputs->inst);
					pthread_mutex_lock(&as[l]);
					break;
				}
			}
		}
		else if(flag==2){
			pthread_mutex_lock(&electric_stage);//to ensure that two musicians do not claim the same stage simultaneously
			//check which stage is free
			for(int i=1; i<=e; i++){
				if(e_stage[i]==0){//check if any performer is performing of the stage
					e_stage[i]++;//increment the individual stage performer counter
					pthread_mutex_unlock(&electric_stage);//unlock the electric stage mutex
					l = i;
					strcpy(electric_m_name[l], inputs->name);
					strcpy(electric_m_inst[l], inputs->inst);
					pthread_mutex_lock(&es[l]);
					break;
				}
			}	
		}
		int k = random_func(t1,t2);
		printf("\033[1;35m%s performing %s at %s stage %d for %d sec\n\n\033[0m", inputs->name, inputs->inst, stg, l, k);
		sleep(k);
		if((a_stage[l]==2 && flag==1) || (e_stage[l]==2 && flag==2)){//if a singer joins the musician during his performance wait for 2 sec
			sleep(2);
		}
		printf("\033[1;34m%s's performance at %s stage %d finished\n\n\033[0m",inputs->name, stg, l);
		if(flag==1){
			pthread_mutex_unlock(&as[l]);
			astc++;
			sem_post(&acoustic);//acoustic stage deallocated
			a_stage[l]=0;
		}
		if(flag==2){
			pthread_mutex_unlock(&es[l]);
			elec++;
			sem_post(&electric);//electric stage deallocated
			e_stage[l]=0;
		}
		sem_post(&stage);//if an electric/acoustic stage is deallocated, we also need to decrement the stage semaphore
		sem_wait(&tshirt);//wait for coordinator to be free to get the tshirt
		printf("\033[1;36m%s collecting t-shirt\n\n\033[0m",inputs->name);
		sleep(2);
		sem_post(&tshirt);//leave the cooridnator so that he can attend other musicians
	}
}

void *only_singers(void *inp){
	struct timespec ts;
	struct musician *inputs = (struct musician*)inp;
	sleep(inputs->time);
	printf("\033[1;37m%s %s arrived\n\n\033[0m",inputs->name,inputs->inst);
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += t;
	int x = sem_timedwait(&singer, &ts);//wait if all the stages have singers or waiting time exceeds t
	if(x==-1){// if the waiting time exceeds t, then leave srujana
		printf("\033[1;32m%s %s left because of impatience\n\n\033[0m",inputs->name, inputs->inst);
	}
	else{
		char stg[50];
		int flag=0;
		if(astc_singer > 0 && elec_singer > 0){//if acoustic stages and electric stages have vacancy for a singer
			int h = rand()%2;
			if(h==0){//acoustic stage chosen
				strcpy(stg, "acoustic");
				flag=1;
				astc_singer--;//decrement the acoustic singer vacancy counter
			}
			else{//electric stage chosen
				strcpy(stg, "electric");
				flag=2;
				elec_singer--;//decrement the electric singer vacancy counter
			}
		}
		else if(astc_singer==0){//if only electric stages have vacancy for singers
			strcpy(stg, "electric");
			flag=2;
			elec_singer--;//decrement the electric singer vacancy counter
		}
		else if(elec_singer==0){//if only electric stages have vacancy for singers
			strcpy(stg, "acoustic");
			flag=1;
			astc_singer--;//decrement the acoustic singer vacancy counter
		}
		int l;
		int var=0; char nm[50];
		if(flag==1){
			pthread_mutex_lock(&acoustic_stage);//to ensure that two musicians do not claim the same stage simultaneously
			//check which stage is free
			for(int i=1; i<=a; i++){
				if(a_stage[i]!=2){//if no singer is present on the stage
					a_stage[i]++;//increment the individual stage performer counter
					pthread_mutex_unlock(&acoustic_stage);//unlock the acoustic stage mutex
					l = i;
					if(a_stage[i]==2 && strcmp(acoustic_m_inst[l],"s")!=0){//if some musician (not singer) is already performing on the stage
						var=2;
						strcpy(nm, acoustic_m_name[l]);

					}
					else{// if no musician is performing of the stage
						var=1;
						astc--;//decrement the number of acoustic stages available
						strcpy(acoustic_m_inst[l], inputs->inst);
						strcpy(acoustic_m_name[l], inputs->name);
						sem_wait(&acoustic);//occupy the acoustic stage
						sem_wait(&stage);//if an acoustic stage is allocated, we also need to increment the stage semaphore
					}
					break;
				}
			}
		}
		else if(flag==2){
			pthread_mutex_lock(&electric_stage);//to ensure that two musicians do not claim the same stage simultaneously
			//check which stage is free
			for(int i=1; i<=e; i++){
				if(e_stage[i]!=2){//if no singer is present on the stage
					e_stage[i]++;//increment the individual stage performer counter
					pthread_mutex_unlock(&electric_stage);//unlock the acoustic stage mutex
					l = i;
					if(e_stage[i]==2 && strcmp(electric_m_inst[l],"s")!=0){//if some musician (not singer) is already performing on the stage
						var=2;
						strcpy(nm, electric_m_name[l]);
					}
					else{
						var=1;
						elec--;//decrement the number of electric stages available
						strcpy(electric_m_inst[l], inputs->inst);
						strcpy(electric_m_name[l], inputs->name);
						sem_wait(&electric);//occupy the electric stage
						sem_wait(&stage);//if an electric stage is allocated, we also need to increment the stage semaphore
					}
					break;
				}
			}		
		}
		if(var==1){// if singer is performing a solo
			int k = random_func(t1,t2);
			printf("\033[1;35m%s performing %s at %s stage %d for %d sec\n\n\033[0m", inputs->name, inputs->inst, stg, l, k);
			sleep(k);
			printf("\033[1;34m%s's performance at %s stage %d finished\n\n\033[0m",inputs->name, stg, l);
			if(flag==1){
				astc++;//increment the available acoustic stage counter
				sem_post(&acoustic);//deallocate the acoustic stage
				a_stage[l]=0;//decrement the individual stage performer counter
				astc_singer++;//increment the available acoustic stage singer counter
			}
			if(flag==2){
				elec++;//increment the available electric stage counter
				sem_post(&electric);//deallocate the electric stage
				e_stage[l]=0;//decrement the individual stage performer counter
				elec_singer++;//increment the available elctric stage singer counter
			}
			sem_post(&stage);//if an electric/acoustic stage is deallocated, we also need to decrement the stage semaphore
		}
		else{// if singer is performing along with some musician
			printf("\033[01;33m%s joined %s's performance. Performance extended by 2 sec.\n\n\033[0m", inputs->name, nm);
			if(flag==1){
				pthread_mutex_lock(&as[l]);//wait till the musician signals to leave
				pthread_mutex_unlock(&as[l]);
			}
			if(flag==2){
				pthread_mutex_lock(&es[l]);//wait till the musician signals to leave
				pthread_mutex_unlock(&es[l]);
			}
			printf("\033[1;34m%s's performance at %s stage %d finished\n\n\033[0m",inputs->name, stg, l);
			if(flag==1){
				a_stage[l]=0;//decrement the individual stage performer counter
				astc_singer++;//increment the available acoustic stage singer counter
			}
			if(flag==2){
				e_stage[l]=0;//decrement the individual stage performer counter
				elec_singer++;//increment the available elctric stage singer counter
			}	
		}
		sem_post(&singer);//decrement the singer semaphore
		sem_wait(&tshirt);//wait for coordinator to be free to get the tshirt
		printf("\033[1;36m%s collecting t-shirt\n\n\033[0m",inputs->name);
		sleep(2);
		sem_post(&tshirt);//leave the cooridnator so that he can attend other musicians
	}
}

int main(){
	srand(time(NULL));//random number seed
	//input
	scanf("%d %d %d %d %d %d %d",&k,&a,&e,&c,&t1,&t2,&t);
	pthread_t musicians[k+3];
	char name[k+3][50], inst[k+3][50]; int time[k+3];
	for(int i=1; i<=k; i++){
		scanf("%s %s %d",name[i], inst[i], &time[i]);
	}
	//initialise mutex, semaphores, variables and arrays
	for(int i=1; i<=a; i++){
		a_stage[i]=0;
	}
	for(int i=1; i<=e; i++){
		e_stage[i]=0;
	}
	pthread_mutex_init(&acoustic_stage, NULL);
	pthread_mutex_init(&electric_stage, NULL);
	for(int i=1; i<=a; i++){
		pthread_mutex_init(&as[i], NULL);
	}
	for(int i=1; i<=a; i++){
		pthread_mutex_init(&es[i], NULL);
	}
	sem_init(&stage, 0, a+e);
	sem_init(&singer, 0, a+e);
	sem_init(&acoustic, 0, a);
	sem_init(&electric, 0, e);
	sem_init(&tshirt, 0, c);
	astc=a, elec=e, astc_singer=a, elec_singer=e;
	//Simulation begins 
	printf("\n");
	printf("\033[1;31mSimulation started\n\n\033[0m");
	//create thread for each musician and singer
	for(int i=1; i<=k; i++){
		struct musician *m = (struct musician*)malloc(sizeof(struct musician)); 
		strcpy(m->name, name[i]);
		strcpy(m->inst, inst[i]);
		m->time = time[i];
		if(strcmp(inst[i],"p")==0 || strcmp(inst[i],"g")==0){
			pthread_create(&musicians[i], NULL, both, (void*)(m));
		}
		else if(strcmp(inst[i],"v")==0){
			pthread_create(&musicians[i], NULL, only_a, (void*)(m));	
		}
		else if(strcmp(inst[i],"b")==0){
			pthread_create(&musicians[i], NULL, only_e, (void*)(m));	
		}
		else if(strcmp(inst[i],"s")==0){
			pthread_create(&musicians[i], NULL, only_singers, (void*)(m));	
		}
	}
	//wait for threads to complete their execution
	for(int i=1; i<=k; i++){
		pthread_join(musicians[i], NULL);
	}
	//Simulation over :D
	printf("\033[1;31mSimulation over\n\n\033[0m");
}