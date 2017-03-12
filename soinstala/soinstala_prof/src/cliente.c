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
#include "cliente.h"
#include "ficheiro.h"

extern struct configuracao Config;

int cliente_executar(int id)
{
  int n,count,ret;
    struct servico Servico;
    int servico_id;
    char buf[100];
    char *result;

    setbuf(stdout,NULL);
    
    n = 0;
    count = 0;
    result = Config.lista_clientes;
    while (n<id) {
      while(Config.lista_clientes[count++] != '\0')
	;
      result = &Config.lista_clientes[count];
      n++;
    }

    servico_id = atoi(result);
    Servico.id = servico_id;
    Servico.cliente = id;

    memoria_pedido_s_escreve(id,&Servico);
    memoria_relatorio_c_le(id,&Servico);

    if(Servico.disponivel == 1) {
      printf("CLIENTE %03d solicitou %d e obteve %d (v:%02d c:%02d t:%.9fs)\n",id,servico_id,Servico.id,Servico.rececionista,Servico.instalador,tempo_diferenca(Servico.hora_servico,Servico.hora_conclusao));
        sprintf(buf,"CLIENTE %03d solicitou %d e obteve %d\n",id,servico_id,Servico.id);
	ret = Servico.id;
    } else {
        printf("CLIENTE %03d solicitou %d mas nao estava disponivel!\n",id,servico_id);
        sprintf(buf,"CLIENTE %03d solicitou %d mas nao estava disponivel!\n",id,servico_id);
	ret = Config.SERVICOS;
    }

    ficheiro_escrever_linha(buf);

    return ret;
}
