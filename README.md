# Programação Concorrente
----------------------------------------------------------------------------------------------------------------------------------------
Repositório criado para a disciplina de programação concorrente UFRJ 
+ Disciplina: Programação Concorrente
+ Profª: Silvana Silvana Rossetto

----------------------------------------------------------------------------------------------------------------------------------------
# Análises realizadas do laboratório 4:
Implementação concorrente do problema dos leitores/escritores compartilhando uma lista.
Apresente sua análise comparativa dos tempos de execução das versões com mutex, rwlock e sua implementação do padrão leitores/escritores.

----------------------------------------------------------------------------------------------------------------------------------------
Executando o programa com mutex : 1 thread 
+ 1ª execução teve tempo de 0.978614 segundos
+ 2ª execução teve tempo de  0.986670 segundos
+ 3ª execução teve tempo de 0.990222 segundos
+ 4ª execução teve tempo de 0.989876 segundos

Executando o programa com mutex : 3 threads
+ 1ª execução teve tempo de  2.032322 segundos
+ 2ª execução teve tempo de  2.032248 segundos
+ 3ª execução teve tempo de 2.046884 segundos
+ 4ª execução teve tempo de 2.046841 segundos

Executando o programa com mutex : 5 threads 
+ 1ª execução teve tempo de  2.859151 segundos
+ 2ª execução teve tempo de  3.160601 segundos
+ 3ª execução teve tempo de 3.099776 segundos
+ 4ª execução teve tempo de 3.207780 segundos

Executando o programa com rwlock: 1 thread
+ 1ª execução teve tempo de  0.991897 segundos
+ 2ª execução teve tempo de  1.005786 segundos
+ 3ª execução teve tempo de 0.982739 segundos
+ 4ª execução teve tempo de 0.987470 segundos

Executando o programa com rwlock: 3 threads
+ 1ª execução teve tempo de  3.719481 segundos
+ 2ª execução teve tempo de  3.711417 segundos
+ 3ª execução teve tempo de 3.753858 segundos
+ 4ª execução teve tempo de 3.675509 segundos

Executando o programa com rwlock: 5 threads
+ 1ª execução teve tempo de  7.785794 segundos
+ 2ª execução teve tempo de  7.903463 segundos
+ 3ª execução teve tempo de 9.257874 segundos
+ 4ª execução teve tempo de 9.535834 segundos

Executando minha versão do programa: 1 thread
+ 1ª execução teve tempo de  1.784085 segundos
+ 2ª execução teve tempo de  1.744507 segundos
+ 3ª execução teve tempo de  1.732300 segundos
+ 4ª execução teve tempo de 1.729340 segundos

Executando minha versão do programa: 3 thread
+ 1ª execução teve tempo de  3.679749 segundos
+ 2ª execução teve tempo de  3.795175 segundos
+ 3ª execução teve tempo de  3.786396 segundos
+ 4ª execução teve tempo de 4.075349 segundos

Executando minha versão do programa: 3 thread
+ 1ª execução teve tempo de  11.550869 segundos
+ 2ª execução teve tempo de 13.672744 segundos
+ 3ª execução teve tempo de  14.961245 segundos
+ 4ª execução teve tempo de 15.090834 segundos

 Observações Gerais:
+ O programa com mutex, embora parece eficiente com uma única thread, sofre no desempenho conforme a adição de mais, acredito que seja devido aos bloqueios das operações de escrita. O programa com rwlock tem um desempenho parecido, sendo melhorzinho com uma thread, mas também sofre problemas de escalabilidade com mais threads.

+ Minha versão personalizada, que combina mutex e condições, parece que ficou ali no ponto intermediário entre elas. A questão de tentar manter um equilíbrio entre quem lê e escreve pode ser boa pra Ela busca equilibrar leitores e escritores, o que é interessante para quando utilizadas muitas threads, mas gera essa questão de escalabilidade também, podendo justificar esse maior tempo ai de processamento do que as demais implementações.  



