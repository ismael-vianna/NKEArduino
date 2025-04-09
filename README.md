# NKEArduino

![GitHub repo size](https://img.shields.io/github/repo-size/iuricode/README-template?style=for-the-badge)
![GitHub language count](https://img.shields.io/github/languages/count/iuricode/README-template?style=for-the-badge)
![GitHub forks](https://img.shields.io/github/forks/iuricode/README-template?style=for-the-badge)
![Bitbucket open issues](https://img.shields.io/bitbucket/issues/iuricode/README-template?style=for-the-badge)
![Bitbucket open pull requests](https://img.shields.io/bitbucket/pr-raw/iuricode/README-template?style=for-the-badge)

> Biblioteca para utilizar o NKE na placa arduino. Desenvolvido pelos alunos de Sistemas Embarcados 24/1.
> 

## ☕ Usando NKEArduino

Para usar NKEArduino, siga estas etapas:

1. Faça o download do projeto
2. Copie e cole a pasta inteira "NKEArduino" na pasta "libraries" de seu Arduino IDE. O caminho padrão no Windows é
```
C:\Users\[nome do usuário do Windows]\Documents\Arduino\libraries\
```
3. Reinicie o Arduino IDE se estiver aberto. Abra seu Arduino IDE.
4. No início de seu projeto importe da seguinte forma o NKE Arduino:
```
#include <nke.h>
```
5. Após incluir a biblioteca, no método Setup(), inclua o método "config()". Este método carrega as configurações do NKE Arduino.

Após estes procedimentos é só iniciar a codificar seu projeto.