#define _XOPEN_SOURCE 600

#include <SDL.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>

#include "timer.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#define TAILLE_MAX 5000

// Return number of elapsed µsec since... a long time ago
static unsigned long get_time (void)
{
  struct timeval tv;

  gettimeofday (&tv ,NULL);

  // Only count seconds since beginning of 2016 (not jan 1st, 1970)
  tv.tv_sec -= 3600UL * 24 * 365 * 46;
  
  return tv.tv_sec * 1000000UL + tv.tv_usec;
}

#ifdef PADAWAN

struct sigaction sa, old;

sigset_t empty, full;

unsigned long  curr[TAILLE_MAX];

unsigned long  tmp_curr;

int i, current_index;

void * obj[TAILLE_MAX];

void * tmp_obj;


//remet les indexes à 0.
void clear_tab(){
	for(int j=0; j<i; j++){
		curr[j] = 0;
		obj[j] = NULL;
		current_index = 0;
		i = 0;
	}
}

//handler de l'alarme
void traitant_alarme(){

  sigaction(SIGALRM, &sa, &old);
  sdl_push_event(obj[current_index]);
  current_index++;
  
	if (current_index != i){
    		timer_set((curr[current_index] - (get_time()))/1000, obj[current_index]);
	}else{
		clear_tab();
	}

}

void *DemonThread(void *p)
{

	//On veut que seul le demon puisse traiter SIGALRM
	pthread_mutex_lock(&mutex);
  	sigaction(SIGALRM, &sa, &old);
  
  	printf("lancement du demon\n");
  	printf("%ld\n", pthread_self()); 

  	while(1){
  		sigsuspend(&empty);
  	}
  	printf("fin du demon");
	pthread_mutex_unlock(&mutex);
 	return NULL;
}

// timer_init returns 1 if timers are fully implemented, 0 otherwise
int timer_init (void)
{
	pthread_t tid;
	
	i = 0;
	current_index = 0;
	
  	sigemptyset(&sa.sa_mask);
  	sa.sa_flags = 0;
  	sa.sa_handler = traitant_alarme;
  
	sigaddset(&full, SIGALRM);
	pthread_sigmask(SIG_BLOCK, &full, NULL);
	pthread_create(&tid, NULL, DemonThread, NULL);
	
  	//pthread_mutex_destroy(&mutex);
  	return 1; // Implementation not ready
  
}

//insert un objet dans les tableaux selon son index.
void insert(int index, unsigned long delay, void * param){

	tmp_curr = curr[index];
	tmp_obj = obj[index];

	obj[index] = param;
	curr[index] = delay;
	
	for(int j=index+1; j<i; j++){
		curr[j] = tmp_curr;
		tmp_curr = curr[j+1];
		obj[j] = tmp_obj;
		tmp_obj = obj[j+1];
	}
	i++;
	for(int j=0; j<i; j++){
		printf("%ld <--> %p\n", curr[j], obj[j]);
	}
	printf("index: %d i: %d\n", current_index, i);
	
}

void timer_set (Uint32 delay, void *param)
{
	struct itimerval timer, curr_timer;

	timer.it_value.tv_sec = delay/1000;
	timer.it_value.tv_usec = (delay%1000)*1000;
	timer.it_interval.tv_sec = 0;
  	timer.it_interval.tv_usec = 0;
  
  	getitimer( ITIMER_REAL, &curr_timer);
  	//priorité de traitement
		if ( delay*1000 < curr_timer.it_value.tv_usec + curr_timer.it_value.tv_sec*1000000){
			insert(current_index, get_time() + delay*1000, param);
			
		}
		//non prioritaire
		else{

				insert(i, get_time() + delay*1000, param);

		}
		//si le timer que nous voulons amorcer est plus long que celui qui est en cours
		if ((curr_timer.it_value.tv_usec != 0 || curr_timer.it_value.tv_sec != 0) && 
			(timer.it_value.tv_sec*1000000 + timer.it_value.tv_usec > curr_timer.it_value.tv_usec + curr_timer.it_value.tv_sec*1000000)){

			timer.it_value.tv_sec = curr_timer.it_value.tv_sec;
			timer.it_value.tv_usec = curr_timer.it_value.tv_usec;
		}

		if(setitimer( ITIMER_REAL, &timer, NULL) == -1){
			//***EXECUTE***
			perror("erreur setitimer()");
			exit(1);
		}

}

//implementation à faire
int timer_cancel (timer_id_t timer_id){
return 1;
}

#endif
