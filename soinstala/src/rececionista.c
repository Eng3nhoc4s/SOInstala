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
#include "rececionista.h"

int rececionista_executar(int id)
{
    int ret;
    struct servico Servico;
    int clientes_atendidos_rececionista=0;

    setbuf(stdout,NULL);

    // Passar pedidos de servico para o instalador
    while(1) {
        ret = memoria_pedido_s_le(id,&Servico);
        if(ret == 1) {
            // servico disponível
	    memoria_pedido_i_escreve(id,&Servico);
	} else if(ret == 2) {
	    // servico indisponível
	    memoria_relatorio_c_escreve(id,&Servico);
        } else
	    // sem clientes
            break;
	clientes_atendidos_rececionista++;
    }

    return clientes_atendidos_rececionista;
}
