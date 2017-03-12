#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <sys/mman.h> //mmap
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <ctype.h>
#include <signal.h>
#include <limits.h>

#include "main.h"
#include "so.h"
#include "tempo.h"

// grupo so046
// -----------

struct timespec t_inicial;
long intervalo_alarme;

extern struct configuracao Config; // configuração da execução do SOinstala

void tempo_iniciar(long intervalo)
{
    //==============================================
    // INICIAR ESTRUTURA t_inicial COM VALOR DE RELOGIO (CLOCK_REALTIME)
	//
	// funções de tempo:
	// - clock_gettime() dá um resultado em nanosegundos
	// - gettimeofday()  dá um resultado em milisegundos
	// como a função clock_gettime() dá um valor mais preciso do que gettimeofday()
	// deve ser usada clock_gettime()
	//
    // fazer:
	// - se intervalo!=0 então intervalo_alarme = intervalo
	// - se intervalo!=0 então chamar tempo_armar_alarme();
	// - iniciar estrutura t_inicial com clock_gettime usando CLOCK_REALTIME

	if(intervalo != 0){
		intervalo_alarme = intervalo;
		tempo_armar_alarme();
	}	

	tempo_registar(&t_inicial);
	
    //so_tempo_iniciar(intervalo);
    //==============================================
}

void tempo_terminar(long intervalo)
{
    //==============================================
    // DESATIVAR ALARME
    //
	// desassociar SIGALRM da função tempo_escrever_log_temporizado
    //so_tempo_terminar(intervalo);
	
	signal(SIGALRM, SIG_IGN);
    //==============================================
}

void tempo_armar_alarme()
{
    //==============================================
    // ARMAR ALARME DE ACORDO COM intervalo_alarme (SIGNAL E SETTIMER)
    //
	// fazer:
	// - associar SIGALRM com a função tempo_escrever_log_temporizado
	// - usar setitimer preenchendo apenas os campos value da estrutura
    //so_tempo_armar_alarme();

	signal(SIGALRM, tempo_escrever_log_temporizado);

	struct itimerval iti; 
	
	iti.it_interval.tv_sec = 0;
	iti.it_interval.tv_usec = 0;
	iti.it_value.tv_sec = 0;
	iti.it_value.tv_usec = intervalo_alarme;
		
	
	setitimer(ITIMER_REAL, &iti , 0);

    //==============================================
}

void tempo_escrever_log_temporizado(int signum)
{
    //==============================================
    // ESCREVER LOG NO ECRAN DE FORMA TEMPORIZADA 
    //
	// rearmar alarme chamando novamente tempo_armar_alarme
	// escrever para o ecrã a informação esperada

	tempo_armar_alarme();
	int i;

	printf("	Servicos:");

	for(i = 0; i < Config.SERVICOS; i++)
		printf("	0%d", i);

	puts("");

	printf("	Stock:");
	
	for(i = 0; i < Config.SERVICOS; i++)
		if(Config.stock[i] > 9)
			printf("	%d", Config.stock[i]);
		else
			printf("	0%d", Config.stock[i]);

	puts("");
	
    //so_tempo_escrever_log_temporizado(signum);
    //==============================================
}

double tempo_diferenca(struct timespec t1, struct timespec t2)
{
    //==============================================
    // CALCULAR A DIFERENCA, EM NANOSEGUNDOS, ENTRE t1 E t2
    //
	// realizar as operações aritméticas necessárias para obter o resultado

	double diff = t2.tv_nsec > t1.tv_nsec ? t2.tv_nsec - t1.tv_nsec : t1.tv_nsec - t2.tv_nsec;

	return diff/1000000000;
    //==============================================
}

double tempo_ate_agora()
{
    //==============================================
    // CALCULAR O INTERVALO DE TEMPO ENTRE t_inicial E O INSTANTE ATUAL
    //
	// fazer:
	// - obter o tempo atual com clock_gettime
	// - chamar tempo_diferenca

	struct timespec t_real;

	clock_gettime(CLOCK_REALTIME, &t_real);
    return tempo_diferenca(t_inicial, t_real);
    //==============================================
}

void tempo_registar(struct timespec *t)
{
    //==============================================
    // REGISTAR O TEMPO ATUAL EM t (CLOCK_REALTIME)
    //
	// usar clock_gettime com CLOCK_REALTIME

	clock_gettime(CLOCK_REALTIME, t);
    //so_tempo_registar(t);
    //==============================================
}

void tempo_instalacao()
{
    //==============================================
    // ADORMECER POR 1 MILISEGUNDO
    //
	// usar usleep

	usleep(1000);

    //so_tempo_instalacao();
    //==============================================
}
