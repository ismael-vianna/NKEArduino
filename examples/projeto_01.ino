/*
*
* Código de Exemplo de Aplicação
* Exemplo 1
* 
*/

#include <nke.h>

volatile int16_t tid0, tid1, tid2, tid3, tid4;
int i, j;
sem_t s0;
sem_t s1;
sem_t s2;
sem_t s3;
int valor = 0;

void produtor() {     
    static int number0 = 0;
    getmynumber(&number0);  
    nkprint("Produtor= %d \n", &number0);
    for (i=0; i<10000; i++) {
        semwait (&s0);
        // Serial.println("prod");
        valor= valor + 2 ;
        //delay(200);
        sempost (&s0);
    }
    nkprint("Valor1= %d \n", &valor);
    sempost (&s3);
    delay(200);
    
    while(1);
}

void consumidor1() {
    static int number1;
    getmynumber(&number1);
    nkprint("Consumidor1= %d \n", &number1);
    nkprint("Valor2= %d \n", &valor);
    for (j=0; j<6000; j++) {
        semwait (&s0);
        //Serial.println("cons1");
        valor = valor + 2;
        sempost (&s0);
    }
    nkprint("Valor2= %d \n", &valor);
    sempost (&s2);
    delay(200);
    while(1);
}
void consumidor2() {
    static int number2 ;
    getmynumber(&number2);
    nkprint("Consumidor2= %d \n", &number2);
    //while (1){
    semwait (&s2);
    semwait (&s3);
    //nkprint("Valor", 0);
    //nkprint(valor, 0);
    nkprint("ValorF= %d \n", &valor);
    //sempost (&s0);
    delay(2000);
    //}
    
    while(1);
}

void setup() {
    config();

    seminit(&s0, 1);
    seminit(&s1, 0);
    seminit(&s2, 0);
    seminit(&s3, 0);

    taskcreate(&tid0,idle,0);
    taskcreate(&tid1,produtor,0);
    taskcreate(&tid2,consumidor1,1);
    taskcreate(&tid3,consumidor2,2);
    start(RR) ; //coloca as tasks na fila

    runKernel();    
}

void loop() {
    while (1);
}
