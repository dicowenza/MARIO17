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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; /* pour les threads*/

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
sigset_t vide, plein; // crée signaux vide 
unsigned long  curr[TAILLE_MAX];
void * obj[TAILLE_MAX];/*Tableaux de pointeurs */
unsigned long  tmp_curr;/* variable temporaire pour sauvegarder la valeur de curr[current_index]*/
void * tmp_obj;/* variable temporaire pour sauvegarder la valeur de obj[current_index]*/
int i, current_index;/* variables qui vont s'incrémenter au fur et à mesure */


//remet le tableau curr à 0 / obj à NULL et current index/i à 0 */
void clear_tab(){
	for(int j=0; j<i; j++){
		curr[j] = 0;
		obj[j] = NULL;
		current_index = 0;
		i = 0;
	}
}

/*Fonction qui gère le traitement de l'alarme */
void traite_alarme(void * param){

  sigaction(SIGALRM, &sa, &old);
  sdl_push_event(obj[current_index]);
  printf ("sdl_push_event(%p) appelée au temps %ld\n", param, get_time ());

  current_index++;
  
	if (current_index != i){
    timer_set((curr[current_index] - (get_time()))/1000, obj[current_index]);
	}else{
		clear_tab();
	}

}

/*On crée un thread qui effectue une boucle infinie en appelant sigsupend */
void * DemonThread(void *p)
{

 /* création du thread , il doit être le seul à pouvoir lancer des SIGALARM ! */
	pthread_mutex_lock(&mutex);
  sigaction(SIGALRM, &sa, &old);
  printf("%ld\n", pthread_self()); // on affiche l'identifiant du thread courant
	/*Boucle infinie qui appel sigsupend  */
  while(1){
  	sigsuspend(&vide);
  }
	/* on libère l'objet référencé par mutex */
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
	/*on appel la fonction qui gère le traitement de l'alarme définie plus haut */
  	sa.sa_handler = traite_alarme;
  
	sigaddset(&plein, SIGALRM);
	pthread_sigmask(SIG_BLOCK, &plein, NULL);
	pthread_create(&tid, NULL, DemonThread, NULL);//Création d'un demon
	
  return 1; 
  
}


//insert un objet dans les tableaux selon la variable index.
void insert(int index, unsigned long delay, void * param){
	/* on sauvegarde les valeurs actuelles */
	tmp_curr = curr[index];
	tmp_obj = obj[index];
	/* on remplace stock dans obj le param et dans curr le délai */
	obj[index] = param;
	curr[index] = delay;
	
	for(int j=index+1; j<i; j++){
		curr[j] = tmp_curr;
		tmp_curr = curr[j+1];
		obj[j] = tmp_obj;
		tmp_obj = obj[j+1];
	}
	i++;//on incrémente i
	
	
}

timer_id_t  timer_set (Uint32 delay, void *param)
{		/* on doit "triés" chronologiquement les jalons  , on va donc traités ceux*/
	struct itimerval timer, curr_timer;

	timer.it_value.tv_sec = delay/1000;
	timer.it_value.tv_usec = (delay%1000)*1000;
	timer.it_interval.tv_sec = 0;
  	timer.it_interval.tv_usec = 0;
  
  getitimer( ITIMER_REAL, &curr_timer);
  	/*On compare les différents temps des événements pour savoir si on utilise l'indice i ou curr_index*/

		
		if ( delay*1000 < curr_timer.it_value.tv_usec + curr_timer.it_value.tv_sec*1000000){
			/* on est prioritaire par rapport à l'événement en cours */
			insert(current_index, get_time() + delay*1000, param);
			
		}
		
		else{
				insert(i, get_time() + delay*1000, param);

		}
		/*si le timer que nous voulons amorcer est plus long que celui qui est en cours il faut changer les valeurs et prendre celle en cours */
		if ((curr_timer.it_value.tv_usec != 0 || curr_timer.it_value.tv_sec != 0) && (timer.it_value.tv_sec*1000000 + timer.it_value.tv_usec > curr_timer.it_value.tv_usec + curr_timer.it_value.tv_sec*1000000)){
			timer.it_value.tv_sec = curr_timer.it_value.tv_sec;
			timer.it_value.tv_usec = curr_timer.it_value.tv_usec;
			
		}

		/*si setitimer == -1 on renvoie une erreur */
		if(setitimer( ITIMER_REAL, &timer, NULL) == -1){
			perror("erreur setitimer()");
			exit(1);
		}

}
int timer_cancel (timer_id_t timer_id)
{
  // TODO

  return 0; // failure
}

#endif
