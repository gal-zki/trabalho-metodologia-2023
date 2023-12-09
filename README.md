# trabalho-metodologia-2023

--------

Esse repositório contém o código do projeto feito na matéria de metodologia de pesquisa.

--------

  Para que o código funcione, é necessário que o usuário possua a biblioteca OpenSSL com opções de desenvolvedor em seu sistema. Essa biblioteca está disponível a partir do comando em: [www.raspberry-projects.com](https://raspberry-projects.com/pi/programming-in-c/security/open-ssl-c-library/installing-openssl-c-library). Com a OpenSSL, será possivel executar as funções relacionadas ao AES.
  
  Também é necessário que o usuário tenha em seu sistema a biblioteca utilizada para executar o Ascon. Para consegui-la, o usuário deve acessar o repositório LibAscon em [github.com](https://github.com/TheMatjaz/LibAscon), disponibilizada por Matjaz, e seguir as instruções de *make* para compilar as dependências utilizadas. Ao fim, o usuário deve encontrar os arquivos **ascon.h** e **libasconfull.a** em sua pasta (além de outros arquivos que não serão utilizados).
  
  Em seguida, os arquivos **ascon.h** e **libasconfull.a** e o arquivo fonte **codigo.c** devem ser colocados na mesma pasta para fazer a execução. Para compilar, basta digitar o comando **gcc codigo.c -o codigo -lm -lssl -lcrypto -libasconfull.a**. O comando gerará um executável chamado codigo. Para executá-lo, use o comando **./codigo a b**, em que a é o tamanho do texto a ser testado e b é o número de amostras a serem feitas.

```
gcc codigo.c -o codigo -lm -lssl -lcrypto -libasconfull.a
```
  
  A saída do codigo é o tempo de cifragem e decifragem do AES e o tempo de cifragem e decifragem do Ascon para cada amostra. Além disso, no fim da saída, é feita a média e o desvio padrão de todos os valores obtidos. Na saída, também são mostrados os textos que foram gerados aleatoriamente e usados para os testes de cada amostra. Mostram-se os textos originais, versão cifrada e decifrada para cada algoritmo. 
  
  Os tempos são todos medidos em microssegundos. Note que, se o tamanho do texto for maior que 50 bytes, o texto gerado para o teste não aparecerá na saída, isso acontece porque a saída seria muito poluída. Outro ponto importante é que, a partir da última atualização da OpenSSL, as funções usadas para esse trabalho receberam um aviso que indica que estão descontinuadas (*-deprecated*). Elas foram usadas apesar dos avisos porque não há substituição disponível no contexto de baixo nível abordado no trabalho.

Observação: adicionei errôneamente o arquivo de licensa e não consegui tirar. Ignore-o.
