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
#include "so.h"
#include "escalonador.h"

struct escalonamento Escalonamento; // estrutura de suporte às decisões de escalonamento

void escalonador_iniciar (int instalador, int servico)
{
    //==============================================
    // REGISTAR QUE O INSTALADOR SABE CONFECIONAR ESTO SERVICO
    //
    // escrever 1 na posição respetiva do vetor Escalonamento.ptr
    so_escalonador_iniciar (instalador, servico);
    //==============================================
}

int escalonador_obter_instalador(int rececionista, int servico)
{
    //==============================================
    // ESCOLHER INSTALADOR PARA CONFECIONAR O SERVICO
    //
    // dada uma servico, escolher um dos instaladores que a saiba
    // confecionar
    // a informação do rececionista que está a pedir um instalador
    // pode não ser necessária
    return so_escalonador_obter_instalador(rececionista, servico);
    //==============================================
}
