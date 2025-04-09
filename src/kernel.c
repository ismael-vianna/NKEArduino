/*
*
* Arquivo Kernel do NKE
* Atualizado em 21 ago 2024
*
* Nesta versão foi incorporado os arquivos 
* config.h, salva.c, syscall.c e usercall.c ao kernel.c
*
* Com os arquivos separados estava ocorrendo erros
* de dependências cruzadas e definições duplicadas,
* mas não identificadas. Por este motivo os
* arquivos acima foram unificados. 
*
* É proposto, para um futuro estudo, a separação
* destes arquivos. Para facilitar a separação de 
* quais métodos pertencem aos arquivos acima, foi
* identificado no decorrer do código o intervalo 
* do texto que pertence.
*
*/

/*
 * Esta biblioteca deve ser instalada pelo Gestor de Bibliotecas
 * do Arduino IDE.
 * Após sua instalação, é possível importar apenas usando o 
 * include abaixo.
 * 
 * Sua referência é:
 * https://www.pjrc.com/teensy/td_libs_TimerOne.html
 *
*/
#include <TimerOne.h>

/*
*
* Arquivo de Configuração do NKE
* Atualizado em 21 ago 2024
* 
* Normas de desenvolvimento neste projeto:
* - As definições em letras maiúsculas como por exemplo #define CLKT 2
* - As variáveis globais iniciam em letras maiúsculas. Se houver mais 
* de uma palavra no nome dela, deve juntá-las e colocar a primeira 
* letra em maiúscula, por exemplo: int  SchedulerAlgorithm 
* - Para variáveis locais segue o mesmo padrão das variáveis globais,
* com exceção da primeira letra do nome completo, que deve ser em letra
* minúscula, como por exemplo: int numberTask
* - Para nomes de métodos segue o mesmo padrão das variáveis locais.
* - Evite usar o caracter underline/underscore nos nomes das variáveis
* globais, locais e dos métodos.
*
*/

/*
 * Adicionar versão do NKE em nível de mudanças:
 * [maior].[média].[pequena]
*/ 

// #ifndef CONFIG_H
// #define CONFIG_H

#define VERSION 0.00.005

/* 
 * Define Clock de interrupção
 * Tabela de Interrupção :
 * 1       CLKT = 1   segundo
 * 0.1     CLKT = 100 milissegundos
 * 0.01    CLKT = 10  milissegundos
 * 0.001   CLKT = 1   milissegundo
 * 0.0001  CLKT = 100 microssegundos
 * 0.00001 CLKT = 10  microssegundos
*/

#define CLKT 2

/*
 * Abaixo estão as taxas de transmição na 
 * porta serial do Arduino. Comente aquelas
 * que não será usado no
*/

#define SERIAL_04K8 4800
#define SERIAL_09K6 9600
#define SERIAL_19K2 19200
#define SERIAL_38K4 38400

#define INVALID_INDEX 0xFFFF
#define SLICE 1000000         //1 segundos
#define MAX_NUMBER_TASK 5
#define NUM_TASKS 4
#define SIZE_TASK_STACK 128   // Tamanho da pilha da tarefa
#define MAX_NKREAD_QUEUE 5    // Número máximo de threads esperando por leitura
#define MAX_NAME_LENGTH 30

unsigned int NumberTaskAdd=-1;
volatile int TaskRunning = 0;
int SchedulerAlgorithm;
char myName[MAX_NAME_LENGTH];

enum Scheduler
{
  RR,
  RM,
  EDF
};

enum Taskstates
{
  INITIAL,
  READY,
  RUNNING,
  DEAD,
  BLOCKED
};

typedef struct
{
    int queue[MAX_NUMBER_TASK];
    int tail;
    int head;
}ReadyList;
ReadyList ready_queue;

typedef struct
{
  short count;
  int sem_queue[MAX_NUMBER_TASK], tail, header;
}sem_t;

typedef struct
{
    int tid;                 // ID da thread esperando pela leitura
    const char *format;      // Formato da entrada esperado (similar ao scanf)
    void *var;               // Argumentos onde os dados serão armazenados
}NkReadQueueEntry;
NkReadQueueEntry nkreadQueue[MAX_NKREAD_QUEUE];

int nkreadQueueHead = 0;
int nkreadQueueTail = 0;
char serialInputBuffer[128]; // Buffer para armazenar a entrada da serial
int serialInputIndex = 0;

typedef struct
{
  int CallNumber;
  unsigned char *p0;
  unsigned char *p1;
  unsigned char *p2;
  unsigned char *p3;
}Parameters;
volatile Parameters kernelargs;

typedef struct
{
  int16_t Tid;
  const char *name;
  unsigned short Prio;
  unsigned int Time;
  unsigned short Join;
  unsigned short State;
  uint8_t Stack[SIZE_TASK_STACK]; // Vetor de pilha
  uint8_t* P;                     // Ponteiro de pilha
} TaskDescriptor;

// Array de descritores de tarefas
TaskDescriptor Descriptors[MAX_NUMBER_TASK];

/* 
*
*Serviços do kernel
*
*/
enum sys_temCall
{
  TASKCREATE,
  SEM_WAIT,
  SEM_POST,
  SEM_INIT,
  WRITELCDN,
  WRITELCDS,
  EXITTASK,
  SLEEP,
  MSLEEP,
  USLEEP,
  LIGALED,
  DESLIGALED,
  START, 
  TASKJOIN,
  SETMYNAME,
  GETMYNAME,
  NKPRINT,
  GETMYNUMBER,
  NKREAD
};
// #endif // CONFIG_H

// Métodos usados no NKE
// Métodos do Kernel
void kernel();
void wakeUP();
void switchTask();
void printReadyList();
void systemContext();
void idle();
void sortReadyList();
void InsertReadyList(int id);

// Métodos de Chamadas de Sistema
void sys_taskcreate(int *tid, void (*taskFunction)(void), int priority);
static inline int calcularPrecisao( float valor);
void sys_nkprint(char *fmt,void *number);
void sys_taskexit(void);
void enqueueNkRead(int tid, const char *format, void *var);
NkReadQueueEntry dequeueNkRead();
void sys_nkread(const char *format, void *var);
float stringToFloat(const char* str);
void serialEvent();
void sys_start(int scheduler);
void sys_getmynumber(int *number);
void sys_ligaled();
void sys_desligaled();
void sys_setmyname(const char *name);
void sys_getmyname(const char *name);
void sys_semwait(sem_t *semaforo);
void sys_sempost(sem_t *semaforo);
void sys_seminit(sem_t *semaforo, int valorInicial);
void sys_sleep(unsigned int segundo);
void sys_msleep(unsigned int mili);
void sys_usleep(unsigned int micro);

// Métodos de Salva e Restaura Contexto
void saveContext(TaskDescriptor* task);
void restoreContext(TaskDescriptor* task);

// Métodos de Chamadas do Usuário
void config(); // Função obrigatória no início de Setup()
void runKernel(); // Função obrigatória no final de Setup()
void callsvc(Parameters *args);
void taskcreate(int *ID, void (*funcao)(), int *Priority);
void start(int scheduler);
void semwait(sem_t *semaforo);
void sempost(sem_t *semaforo);
void seminit(sem_t *semaforo, int ValorInicial);
void setmyname(const char *name);
void getmynumber(int *number);
void getmyname(const char *name);
void sleep(int time);
void msleep(int time);
void usleep(int time);
void taskexit(void);
void ligaled(void);
void desligaled(void);
void nkprint(char *fmt,void *number);
void nkread(const char *format, void *var);

/*
* Rotinas do kernel
*/
void kernel()
{
  switch(kernelargs.CallNumber){
    case TASKCREATE: // OK
      sys_taskcreate((int *)kernelargs.p0,(void(*)())kernelargs.p1,(int *)kernelargs.p2);
      break;
    case SEM_WAIT: // OK
     // Serial.println("SEMWAIT: ") ;
      sys_semwait((sem_t *)kernelargs.p0);
      break;
    case SEM_POST: // OK
      sys_sempost((sem_t *)kernelargs.p0);
      break;
    case SEM_INIT: // OK
      //Serial.println("SEMINIT: ") ;
      sys_seminit((sem_t *)kernelargs.p0,(int)kernelargs.p1);
      break;
    case WRITELCDN: // NAO TEREMOS
      // LCDcomando((int)arg->p1);
      // LCDnum((int)arg->p0);
      break;
    case WRITELCDS: // NAO TEREMOS
      // LCDcomando((int)arg->p1);
      // LCDputs((char*)arg->p0);
      break;
    case EXITTASK: // OK
      sys_taskexit();
      break;
    case SLEEP: 
      sys_sleep((int)kernelargs.p0);
      break;
    case MSLEEP: 
      sys_msleep((int)kernelargs.p0);
      break;
    case USLEEP: 
      sys_usleep((int)kernelargs.p0);
      break;
    case LIGALED: // OK
      sys_ligaled();
      break;
    case DESLIGALED: // OK
      sys_desligaled();
      break;
    case START: 
      sys_start((int)kernelargs.p0);
      break;
    case TASKJOIN: 
     // sys_taskjoin((int)arg->p0);
      break;
    case SETMYNAME: // OK
      sys_setmyname((const char *)kernelargs.p0);
      break;
    case GETMYNAME: // OK
      sys_getmyname((const char *)kernelargs.p0);
      break;  
    case NKPRINT: // OK
       sys_nkprint((char *)kernelargs.p0,(void *)kernelargs.p1);
       break;
    case GETMYNUMBER: 
       sys_getmynumber((int *)kernelargs.p0);
       break;
    case NKREAD: 
      //  sys_nkread((char *)kernelargs->p0,(void *)kernelargs->p1); //alterado de arg para kernelargs
       sys_nkread((char *)kernelargs.p0,(void *)kernelargs.p1);
       break;
    default:
       break;
  }
}

/*
* Acorda a task bloqueada a espera de passagem de tempo
*/
void wakeUP()
{
  int i=1;
  for(i=1;i<=NUM_TASKS; i++){
    //sleep
    if(Descriptors[i].Time>0){
      Descriptors[i].Time--;
      if(Descriptors[i].Time <= 0 && Descriptors[i].State == BLOCKED){
        Descriptors[i].State = READY;
        InsertReadyList(i); //tempo de espera se esgotou
      }
    }
  }
}

/*
*   
* Se a task atual não é Idle (TaskRunning != 0), a task é removida da Ready List
* Caso não esteja bloqueada, ela é reinserida no final da Ready List
* A remoção é feita com o deslocamento para a esquerda 
* Atualiza TaskRunning com a primeira task da Ready List (TaskRunning = ready_queue.queue[0])
* Se a Ready List estiver vazia, TaskRunning será 0 (Idle) 
*
*/
void switchTask()
{
  saveContext(&Descriptors[TaskRunning]);

  if (TaskRunning != 0){
    for (int i = 0; i < ready_queue.head - 1; i++) {
      ready_queue.queue[i] = ready_queue.queue[i + 1];
    }
    ready_queue.head--;
    if (Descriptors[TaskRunning].State != BLOCKED)  {   
      InsertReadyList(TaskRunning);   
    }
  }
  
  if (ready_queue.head > 0){
    TaskRunning = ready_queue.queue[0];
  } else {
    TaskRunning = 0;
  }
  
  Descriptors[TaskRunning].State = RUNNING;
  restoreContext(&Descriptors[TaskRunning]);
}

// /*
// * Escalonador: Chamada na interrupção do timer
// * Remover método abaixo, pois não está sendo usado
// * Trata a interrupção do Timer
// */
// void switchTask() 
// {
//   saveContext(&Descriptors[TaskRunning]) ;
//   TaskRunning = (TaskRunning + 1) % NumberTaskAdd ;
//   while(Descriptors[TaskRunning].State == BLOCKED) {
//      TaskRunning = (TaskRunning + 1) % NUM_TASKS ;
//   }
//   restoreContext(&Descriptors[TaskRunning]);
// }

/*
* Imprime o conteúdo da Ready List
* Usada para verificação durante testes
*/
 void printReadyList() {
    Serial.print("Ready list tasks: ");
    for (int i = 0; i < ready_queue.head; i++) {
        Serial.print(" Index:");
        Serial.print(ready_queue.queue[i]);
    }
    Serial.println(" ");
}

/*
* Chamada pela interrupção do Timer
*/
void systemContext() 
{
  wakeUP();
  serialEvent();
  switchTask();
}

/*
* Idle Process
*/
void idle() 
{
  while (1){
  };
}

/*
*   
* Algoritmo Bubble Sort para a reordenação da Ready List
* O critério de ordenação é a prioridade (Prio) definida para a task
* Menor valor númerico indica maior prioridade
*
*/
void sortReadyList() {
  for (int i = 0; i < ready_queue.head - 1; i++) {
      for (int j = 0; j < ready_queue.head - i - 1; j++) {
          if (Descriptors[ready_queue.queue[j]].Prio > Descriptors[ready_queue.queue[j + 1]].Prio) {
              int temp = ready_queue.queue[j];
              ready_queue.queue[j] = ready_queue.queue[j + 1];
              ready_queue.queue[j + 1] = temp;
          }
      }
  }
}

/*
* A inserção da task é inicialmente realizada no final da Ready List
* É chamada sortReadyList() para reordenação por prioridade
*/
void InsertReadyList(int id) {
  ready_queue.queue[ready_queue.head] = id;
  ready_queue.head++;
  sortReadyList();
}

// void InsertReadyList(int id) 
// {
//   // nkprint("Insert: ", 0);
//   switch (SchedulerAlgorithm){
//   case RR:
//     if (ReadyQueue.head == ReadyQueue.tail && ReadyQueue.queue[ReadyQueue.head] == INVALID_INDEX){ // Fila vazia
//       ReadyQueue.tail = 0;
//       ReadyQueue.head = 0;
//       ReadyQueue.queue[ReadyQueue.head] = id;
//     }else{
//       // Atualiza o tail circularmente
//       ReadyQueue.tail = (ReadyQueue.tail + 1) % MAX_NUMBER_TASK;
//       ReadyQueue.queue[ReadyQueue.tail] = id;
//       Descriptors[id].State == READY;
//     }
//     break;
//   default:
//     break;
//   }
// }

/*
*
* Arquivo de Chamadas de Sistema do NKE
* Atualizado em 21 ago 2024
* A partir deste ponto é tratado SysCall
*/
// #include <src/.h>
// #include <src/kernel.c>

/*
* Sys Call
*/
void sys_taskcreate(int *tid, void (*taskFunction)(void), int priority)
{
  NumberTaskAdd++;
  *tid = NumberTaskAdd;
  Descriptors[NumberTaskAdd].Tid = *tid;
  Descriptors[NumberTaskAdd].State = READY;
  Descriptors[NumberTaskAdd].Join = 0;
  Descriptors[NumberTaskAdd].Time = 0;
  Descriptors[NumberTaskAdd].Prio = priority;
  uint8_t *stack = Descriptors[*tid].Stack + SIZE_TASK_STACK - 1;
  Descriptors[*tid].P = stack;

  *(stack--) = ((uint16_t)taskFunction) & 0xFF;      // PC low byte
  *(stack--) = ((uint16_t)taskFunction >> 8) & 0xFF; // PC high byte
  *(stack--) = 0x00;                                 // R0
  *(stack--) = 0x80;                                 // SREG with global interrupts enabled

  for (int i = 1; i < 32; i++){
    *(stack--) = i; // Initialize all other registers with their number
  }

  Descriptors[*tid].P = stack;
}

/*
*  calcularPrecisao( float valor) chamada pela sys_nkprint
*/
static inline int calcularPrecisao( float valor)
{
  int precisaoFloatArduino = 6;
  int precisao = 0;
  int valorInteiro = (int)valor;
  while(valorInteiro > 0){
    valorInteiro = valorInteiro / 10;
    precisao++;
  }
  return precisaoFloatArduino - precisao;
}

/*
* 
*/
void sys_nkprint(char *fmt,void *number)
{
  int *auxint;
  float *auxfloat;
  char *auxchar;
  int size=0;
  int accuracy=1;

  while (*fmt){
    switch (*fmt){
    case '%':
      fmt++;
      switch (*fmt){
        case '%':
          Serial.print(*fmt);
          break;
        case 'c':
          Serial.print((char *)number);
          break;
        case 's':
          sys_nkprint((char *)number, 0);
          break;
        case 'd':
          auxint = number;
          Serial.print(*auxint);
          break;
        case 'f':
          auxfloat = number;
          int precisao = calcularPrecisao(*auxfloat);
          Serial.print(*auxfloat, precisao);
          break;
          // case '.':
          //   fmt++;
          //   while(*fmt != 'f')
          //   {
          //     size*=accuracy;
          //     size+= (*fmt - '0');
          //     accuracy*=10;
          //     fmt++;
          //   }
          //   auxfloat=number;
          //   // printfloat(*auxfloat,size);
          //   break;
          // case 'x':
          //   auxint=number;
          //   // printhexL(*auxint);
          //   break;
          // case 'X':
          //   auxint=number;
          //   // printhexU(*auxint);
          //   break;
          // case 'b':
          //   fmt++;
          //   switch(*fmt)
          //   {
          //     case 'b':
          //       size=8;
          //       break;
          //     case 'w':
          //       size=16;
          //       break;
          //     case 'd':
          //       size=32;
          //       break;
          //     default:
          //       fmt -= 2;
          //       size = 32;
          //       break;
          //   }
          //   auxint=number;
          //   // printbinary(*auxint, size);
          //   break;
        default:
          break;
      }
      break;
    case '\\':
      fmt++;
      if (*fmt == 'n'){
        Serial.print("\n");
      }else{
        Serial.print("\\");
      }
      break;
    default:
      Serial.print(*fmt);
      break;
    }
    fmt++;
  }
}

/*
* 
*/
void sys_taskexit(void)
{
  Descriptors[TaskRunning].State=BLOCKED;
  switchTask();
}

/*
* 
*/
void enqueueNkRead(int tid, const char *format, void *var)
{
  nkreadQueue[nkreadQueueTail].tid = tid;
  nkreadQueue[nkreadQueueTail].format = format;
  nkreadQueue[nkreadQueueTail].var = var;
  nkreadQueueTail = (nkreadQueueTail + 1) % MAX_NKREAD_QUEUE;
}

/*
* 
*/
NkReadQueueEntry dequeueNkRead()
{
  NkReadQueueEntry entry = nkreadQueue[nkreadQueueHead];
  nkreadQueueHead = (nkreadQueueHead + 1) % MAX_NKREAD_QUEUE;
  return entry;
}

/*
* 
*/
void sys_nkread(const char *format, void *var)
{
  // Adicionar a thread atual na fila de leitura
  enqueueNkRead(Descriptors[TaskRunning].Tid, format, var);

  // Bloquear a thread atual
  Descriptors[TaskRunning].State = BLOCKED;
  switchTask();
}

/*
* 
*/
float stringToFloat(const char* str)
{
  float result = 0.0;
  float factor = 1.0;

  if (*str == '-'){
    str++;
    factor = -1.0;
  }

  // Parte inteira
  for (; *str >= '0' && *str <= '9'; str++){
    result = result * 10.0 + (*str - '0');
  }

  // Parte fracionária
  if (*str == '.'){
    float fraction = 0.1;
    str++;
    for (; *str >= '0' && *str <= '9'; str++){
      result += (*str - '0') * fraction;
      fraction *= 0.1;
    }
  }

  return result * factor;
}

/*
* 
*/
void serialEvent()
{
  while (Serial.available()){
    char c = Serial.read();
    if (c == '\n'){
      // Termina a string
      serialInputBuffer[serialInputIndex] = '\0';

      // Reinicia o índice
      serialInputIndex = 0;

      // Desbloquear a thread que está esperando por entrada
      if (nkreadQueueHead != nkreadQueueTail){
        NkReadQueueEntry entry = dequeueNkRead();
        if (strcmp(entry.format, "%f") == 0){
          // Para float, usar nossa função auxiliar
          *(float *)(entry.var) = stringToFloat(serialInputBuffer);
        }else{
          // Interpretar a entrada de acordo com o formato fornecido
          sscanf(serialInputBuffer, entry.format, entry.var);
        }
        Descriptors[entry.tid].State = READY;
      }
    }else{
      if (serialInputIndex < 127){
        serialInputBuffer[serialInputIndex++] = c;
      }
    }
  }
}

/*
* 
*/
void sys_start(int scheduler)
{
  int i;
  SchedulerAlgorithm = scheduler;
  switch (SchedulerAlgorithm){
  case RR:
    for (i = 0; i <= NumberTaskAdd; i++){
      InsertReadyList(i);
    }
    break;
  default:
    break;
  }
}

/*
* 
*/
void sys_getmynumber(int *number)
{
  *number=Descriptors[TaskRunning].Tid;
}

/*
* 
*/
void sys_ligaled()
{
  PORTB = PORTB | 0x20;
}

/*
* 
*/
void sys_desligaled()
{
  PORTB = PORTB & 0xDF;
}

/*
* 
*/
void sys_setmyname(const char *name)
{
  Descriptors[TaskRunning].name=name;
}

/*
* 
*/
void sys_getmyname(const char *name)
{
  strcpy(name, Descriptors[TaskRunning].name);
}

/*
* 
*/
void sys_semwait(sem_t *semaforo)
{
  semaforo->count--;
  if (semaforo->count < 0){
    semaforo->sem_queue[semaforo->tail] = TaskRunning;
    Descriptors[TaskRunning].State = BLOCKED;
    semaforo->tail++;
    if (semaforo->tail == MAX_NUMBER_TASK - 1)
      semaforo->tail = 0;
    switchTask();    
  }
}

/*
* 
*/
void sys_sempost(sem_t *semaforo)
{
  semaforo->count++;
  if (semaforo->count <= 0){
    Descriptors[semaforo->sem_queue[semaforo->header]].State = READY;
    InsertReadyList(semaforo->sem_queue[semaforo->header]);
    semaforo->header++;
    if (semaforo->header == MAX_NUMBER_TASK - 1)
      semaforo->header = 0;
  }
}

/*
* 
*/
void sys_seminit(sem_t *semaforo, int valorInicial)
{
  semaforo->count = valorInicial;
  semaforo->header = 0;
  semaforo->tail = 0;
}

/*
* 
*/
void sys_sleep(unsigned int segundo)
{
  //Descriptors[TaskRunning].Time = segundo/CLKT;
  Descriptors[TaskRunning].Time = (segundo*1000000)/SLICE;
  if(Descriptors[TaskRunning].Time > 0){
    Descriptors[TaskRunning].State = BLOCKED;
    switchTask();
  }
}

/*
* 
*/
void sys_msleep(unsigned int mili)
{
  Descriptors[TaskRunning].Time = (mili/CLKT)/1000;
  if(Descriptors[TaskRunning].Time > 0)
  {
    Descriptors[TaskRunning].State = BLOCKED;
    switchTask();
  }
}

/*
* 
*/
void sys_usleep(unsigned int micro)
{
  Descriptors[TaskRunning].Time = (micro/CLKT)/1000000;
  if(Descriptors[TaskRunning].Time > 0)
  {
    Descriptors[TaskRunning].State = BLOCKED;
    switchTask();
  }
}

/*
*
* Arquivo Salva e Restaura Contexto do NKE
* Atualizado em 21 ago 2024
*
*/

/*
*
*/
void saveContext(TaskDescriptor* task)
{
    asm volatile(
        "push r0 \n\t"
        "in r0, __SREG__ \n\t"
        "cli \n\t"
        "push r0 \n\t"
        "push r1 \n\t"
        "clr r1 \n\t"
        "push r2 \n\t"
        "push r3 \n\t"
        "push r4 \n\t"
        "push r5 \n\t"
        "push r6 \n\t"
        "push r7 \n\t"
        "push r8 \n\t"
        "push r9 \n\t"
        "push r10 \n\t"
        "push r11 \n\t"
        "push r12 \n\t"
        "push r13 \n\t"
        "push r14 \n\t"
        "push r15 \n\t"
        "push r16 \n\t"
        "push r17 \n\t"
        "push r18 \n\t"
        "push r19 \n\t"
        "push r20 \n\t"
        "push r21 \n\t"
        "push r22 \n\t"
        "push r23 \n\t"
        "push r24 \n\t"
        "push r25 \n\t"
        "push r26 \n\t"
        "push r27 \n\t"
        "push r28 \n\t"
        "push r29 \n\t"
        "push r30 \n\t"
        "push r31 \n\t"
        "in %A0, __SP_L__ \n\t"
        "in %B0, __SP_H__ \n\t"
        : "=r"(task->P));
}

/*
*
*/
void restoreContext(TaskDescriptor* task)
{
    asm volatile(
        "out __SP_L__, %A0 \n\t"
        "out __SP_H__, %B0 \n\t"
        "pop r31 \n\t"
        "pop r30 \n\t"
        "pop r29 \n\t"
        "pop r28 \n\t"
        "pop r27 \n\t"
        "pop r26 \n\t"
        "pop r25 \n\t"
        "pop r24 \n\t"
        "pop r23 \n\t"
        "pop r22 \n\t"
        "pop r21 \n\t"
        "pop r20 \n\t"
        "pop r19 \n\t"
        "pop r18 \n\t"
        "pop r17 \n\t"
        "pop r16 \n\t"
        "pop r15 \n\t"
        "pop r14 \n\t"
        "pop r13 \n\t"
        "pop r12 \n\t"
        "pop r11 \n\t"
        "pop r10 \n\t"
        "pop r9 \n\t"
        "pop r8 \n\t"
        "pop r7 \n\t"
        "pop r6 \n\t"
        "pop r5 \n\t"
        "pop r4 \n\t"
        "pop r3 \n\t"
        "pop r2 \n\t"
        "pop r1 \n\t"
        "pop r0 \n\t"
        "out __SREG__, r0 \n\t"
        "pop r0 \n\t"
        : : "r"(task->P));
}

/*
*
* Arquivo de Chamadas do Usuário do NKE
* Atualizado em 21 ago 2024
*
*/

/*
* Este método deve ser chamado no início do método Setup do projeto do Arduino IDE
*/
void config()
{
  Serial.begin(SERIAL_09K6);

  nkprint("Exemplo Projeto NKE \n", 0);
  nkprint("Versao 0.0.5 \n", 0);

  noInterrupts(); 
  Timer1.initialize(SLICE);
  Timer1.attachInterrupt(systemContext);
  interrupts();
}

/*
* Este método deve ser chamado no final do método Setup do projeto do Arduino IDE
*/
void runKernel()
{ 
  noInterrupts();
  restoreContext(&Descriptors[0]);
}    

/*
* Passa a executar a rotina do kernel com interrupções desabilitadas
*/
void callsvc(Parameters *args)
{
  noInterrupts();
  kernelargs = *args;
  kernel();
  interrupts();
}

/*
* Cria tarefa (User Call)
* Parâmetros armazenados em R0 e R1 na chamada
*/
void taskcreate(int *ID,void (*funcao)(), int *Priority) 
{
  Parameters arg;
  arg.CallNumber=TASKCREATE;
  arg.p0=(unsigned char *)ID;
  arg.p1=(unsigned char *)funcao;
  arg.p2=(unsigned char *)Priority;
  callsvc(&arg);
}

/*
*
*/
void start(int scheduler)
{
  Parameters arg;
  arg.CallNumber=START;
  arg.p0=(unsigned char *)scheduler;
  callsvc(&arg);
}

/*
*
*/
void semwait(sem_t *semaforo)
{
  Parameters arg;
  arg.CallNumber=SEM_WAIT;
  arg.p0=(unsigned char *)semaforo;
  callsvc(&arg);
}

/*
*
*/
void sempost(sem_t *semaforo)
{
  Parameters arg;
  arg.CallNumber=SEM_POST;
  arg.p0=(unsigned char *)semaforo;
  callsvc(&arg);
}

/*
*
*/
void seminit(sem_t *semaforo, int ValorInicial)
{
  Parameters arg;
  arg.CallNumber=SEM_INIT;
  arg.p0=(unsigned char *)semaforo;
  arg.p1=(unsigned char *)ValorInicial;
  callsvc(&arg);
}

/*
*
*/
void setmyname(const char *name)
{
  Parameters arg;
  arg.CallNumber=SETMYNAME;
  arg.p0=(unsigned char *)name;
  callsvc(&arg);
}

/*
*
*/
void getmynumber(int *number)
{
  Parameters arg;
  arg.CallNumber=GETMYNUMBER;
  arg.p0=(unsigned char *)number;
  callsvc(&arg);
}

/*
*
*/
void getmyname(const char *name)
{
  Parameters arg;
  arg.CallNumber=GETMYNAME;
  arg.p0=(unsigned char *)name;
  callsvc(&arg);
}

/*
*
*/
void sleep(int time)
{
  Parameters arg;
  arg.CallNumber=SLEEP;
  arg.p0=(unsigned char *)time;
  callsvc(&arg);
}

/*
*
*/
void msleep(int time)
{
  Parameters arg;
  arg.CallNumber=MSLEEP;
  arg.p0=(unsigned char *)time;
  callsvc(&arg);
}

/*
*
*/
void usleep(int time)
{
  Parameters arg;
  arg.CallNumber=USLEEP;
  arg.p0=(unsigned char *)time;
  callsvc(&arg);
}

/*
*
*/
void taskexit(void)
{
  Parameters arg;
  arg.CallNumber=EXITTASK;
  callsvc(&arg);
}

/*
*
*/
void ligaled(void)
{
  Parameters arg;
  arg.CallNumber=LIGALED;
  callsvc(&arg);
}

/*
*
*/
void desligaled(void)
{
  Parameters arg;
  arg.CallNumber=DESLIGALED;
  callsvc(&arg);
}

/*
* Função de escrita no monitor
*/
void nkprint(char *fmt,void *number)
{
  Parameters arg;
  arg.CallNumber=NKPRINT;
  arg.p0=(unsigned char *)fmt;
  arg.p1=(unsigned char *)number;
  callsvc(&arg);
}

/*
* Função de leitura pelo monitor
*/
void nkread(const char *format, void *var) 
{
    Parameters arg;
    arg.CallNumber = NKREAD;
    arg.p0 = (unsigned char *)format;
    arg.p1 = (unsigned char *)var;
    callsvc(&arg);
}
