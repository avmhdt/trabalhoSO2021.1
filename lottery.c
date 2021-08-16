/*
*  lottery.c - Implementacao do algoritmo Lottery Scheduling e sua API
*
*  Autores: SUPER_PROGRAMADORES_C
*  Projeto: Trabalho Pratico I - Sistemas Operacionais
*  Organizacao: Universidade Federal de Juiz de Fora
*  Departamento: Dep. Ciencia da Computacao
*
*
*/

#include "lottery.h"
#include <stdio.h>
#include <string.h>

//Nome unico do algoritmo. Deve ter 4 caracteres.
const char lottName[]="LOTT";

//=====Funcoes Auxiliares=====

// Retorna inteiro aleatório entre [a, b)
int getRandom(int a, int b) {
    return a + rand() % b;
}

int min(int a , int b) {
    if(a < b) {
        return a;
    }
    return b;
}

int getNumTickets(LotterySchedParams *params) {
    return params->num_tickets;
}

void setNumTickets(LotterySchedParams *params, int tickets) {
    params->num_tickets = tickets;
}

int getTotalAvailableTickets(Process *plist) {
    Process *current = plist;
    LotterySchedParams *params;
    int totalTickets = 0;
    int status;
    while(current) {
        params = processGetSchedParams(current);
        status = processGetStatus(current);
        if(status == PROC_READY || status == PROC_RUNNING) {
            totalTickets += getNumTickets(params);
        }
        current = processGetNext(current);
    }
    return totalTickets;
}

//=====Funcoes da API=====

//Funcao chamada pela inicializacao do S.O. para a incializacao do escalonador
//conforme o algoritmo Lottery Scheduling
//Deve envolver a inicializacao de possiveis parametros gerais
//Deve envolver o registro do algoritmo junto ao escalonador
void lottInitSchedInfo() {
	//...
	SchedInfo *lottSchedInfo;
	lottSchedInfo = malloc(sizeof(SchedInfo));
	strcpy(lottSchedInfo->name, lottName);
	lottSchedInfo->initParamsFn = lottInitSchedParams;
	lottSchedInfo->releaseParamsFn = lottReleaseParams;
	lottSchedInfo->scheduleFn = lottSchedule;

	schedRegisterScheduler(lottSchedInfo);
}

//Inicializa os parametros de escalonamento de um processo p, chamada
//normalmente quando o processo e' associado ao slot de Lottery
void lottInitSchedParams(Process *p, void *params) {
	//...
	processSetSchedParams(p, params);
}

//Retorna o proximo processo a obter a CPU, conforme o algortimo Lottery
Process* lottSchedule(Process *plist) {
	//...
    int availableTickets = getTotalAvailableTickets(plist);
    int counter = 0;
    int winner = getRandom(0, availableTickets);
    int status;
    Process *current = plist;
    void *currentParams;
    while(current) {
        currentParams = processGetSchedParams(current);
        status = processGetStatus(current);
        if(status == PROC_READY || status == PROC_RUNNING) {
            counter += getNumTickets(currentParams);
            if(counter > winner)
                break;
        }
        current = processGetNext(current);
    }
    return current;
}

//Libera os parametros de escalonamento de um processo p, chamada
//normalmente quando o processo e' desassociado do slot de Lottery
//Retorna o numero do slot ao qual o processo estava associado
int lottReleaseParams(Process *p) {
	//...
	processSetSchedParams(p, NULL);
	free(processGetSchedParams(p));
	return processGetSchedSlot(p);
}

//Transfere certo numero de tickets do processo src para o processo dst.
//Retorna o numero de tickets efetivamente transfeirdos (pode ser menos)
int lottTransferTickets(Process *src, Process *dst, int tickets) {
	//...
	LotterySchedParams *srcParams, *dstParams;
	int srcTickets, dstTickets, actualTickets;
	srcParams = processGetSchedParams(src);
	dstParams = processGetSchedParams(dst);
	srcTickets = getNumTickets(srcParams);
	dstTickets = getNumTickets(dstParams);
    actualTickets = min(srcTickets, tickets);
    srcTickets -= actualTickets;
    dstTickets += actualTickets;
    setNumTickets(srcParams, srcTickets);
    setNumTickets(dstParams, dstTickets);
    processSetSchedParams(src, srcParams);
    processSetSchedParams(dst, dstParams);

	return actualTickets;
}
