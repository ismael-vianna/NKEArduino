/*
 * Biblioteca do Sistema RTOS NKE pela UERGS
 * 
 * Versão 0.00.004 2024_07_25 [Beta]
 * 
 * Notas da versão: 
 * Possui uma implementação simplificada 
 * do RR e o nkread está em testes. 
 * 
 * Estrutura do código atualizado em 17 jul 2024, por 
 * Ismael Vianna. Separação dos documentos em arquivos 
 * distintos e montagem no formato de biblioteca para 
 * Arduino IDE. 
 * Alterado partes pequenas como adição do método 
 * 'config()'. Este deve ser colocado dentro do método 
 * Setup() de cada projeto feito em Arduino IDE.
 *   
 *   
*/

#include <avr/io.h>
// #include <Arduino.h>
// #include <src/TimerOne-1.1.1/TimerOne.h>

/* 
 * As referências a seguir pertencem ao projeto 
 * de desenvolvimento acadêmico NKE Arduino, pela
 * UERGS - Universidade do Estado do Rio Grande 
 * do Sul.
*/

#include <src/kernel.c>

