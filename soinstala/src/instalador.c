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
#include <signal.h>
#include <limits.h>

#include "main.h"
#include "memoria.h"
#include "tempo.h"
#include "instalador.h"

extern struct indicadores Ind;
extern struct configuracao Config;

int instalador_executar(int id)
{
    int n;
    struct servico Servico;
    int clientes_atendidos_pelo_instalador=0;

    setbuf(stdout,NULL);

    // Preparar servicos
    while(1) {
        // aguardar pedido do rececionista
        if(memoria_pedido_i_le(id,&Servico) == 0)
            break;
	// preparar servico e aguardar instalacao
	tempo_instalacao();
	// colocar relatorio de conclusao
    memoria_relatorio_c_escreve(id,&Servico);
	Ind.servicos_realizados_pelos_instaladores[Servico.id*Config.INSTALADORES+id]++;
	clientes_atendidos_pelo_instalador++;
    }
    //    printf("INSTALADOR %02d terminou\n",id);
    return clientes_atendidos_pelo_instalador;
}
