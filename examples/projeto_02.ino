/*
*
* Código de Exemplo de Aplicação
* Exemplo 2
* 
*/

#include <nke.h>

volatile int16_t tid0, tid1, tid2, tid3, tid4;
int i, j;
int valor = 0;

void p0()
{     
  static int number0;
  getmynumber(&number0);  
  while (1) {
    printReadyList();
    //nkprint("P0 will sleep: ", 0);
    sleep(5);
    nkprint("P0 running: ", 0);
  }
}

void p1()
{
  static int number1 ;
  getmynumber(&number1);
  while (1) {
    printReadyList();
    //nkprint("P1 will sleep: ", 0);
    sleep(10);
    nkprint("P1 running: ", 0);
  }
}

void p2()
{
  static int number2;
  getmynumber(&number2);
  while (1) {
    printReadyList();
    //nkprint("P2 will sleep: ", 0);
    sleep(20);
    nkprint("P2 running: ", 0);
  }
}

void setup()
{
  config();

  taskcreate(&tid0,idle,0);
  taskcreate(&tid1,p0,0);
  taskcreate(&tid2,p1,1);
  taskcreate(&tid2,p2,2);
  start (RR) ; //coloca as tasks na fila

  runKernel();
}

void loop()
{
  while (1);
}
