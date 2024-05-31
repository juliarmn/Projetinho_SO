# Projeto de SO - primeiro integrável

A ideia do projeto era simular o funcionamento de um sistema operacional. Cada pedaço do que foi requerido foi subdividido em pastas e módulos para  melhor gerenciamento.


As partes em geral são:

1. **Processo:** leitura de arquivo do processo e criação na memória.

2. **Memória:** armazenamento do arquivo de processo em segmentos e páginas, simulando uma memória de 1 Gigabyte. Possui algoritmos de geranciamento do espaço e swap out como o segunda chance.

3. **Escalonador:** organiza a fila na CPU e aplica o round robin, chamando as funções de semáforo.

4. **Semáforo:** funções para tratamento de semáforos como P e V.

5. **Interface:** interação com o usuário.

Usou-se a *pthread* para pode rodar o escalonador juntamente com os outros processos.

## Pré-requisitos

Ter a *ncurses* instalada para pode rodar a interface. 

Para instalar a *ncurses*:

### UBUNTU

Basta rodar no terminal:

    sudo apt update
    sudo apt install libncurses5-dev libncursesw5-dev

### GNOME

Basta rodar:

    sudo dnf install ncurses-devel

Caso não tenha *Cmake*, faça:

### UBUNTU

    sudo apt install cmake

### GNOME

    sudo dnf install cmake

## Compilação com Cmake

Vá até o diretório do projeto  e apague os arquivos de cache e o Makefile com:

    rm -r CMakeCache.txt
    rm -r Makefile

Ou delete por meio da interface ambos os arquivos.

Coloque no terminal:

    cmake .

O comando acima irá originar o Makefile correto

Para compilar o código em si use:

    make

Por fim, para rodar basta inserir

    ./GatuOS

## Sobre o código

### Processo

Para o código do processo, tem-se a função que faz a leitura no arquivo para criar o processo em memória.

Além disso, há a manipulação da lista de instruções para inserir ela no processo.

### Memória

A memória cria uma lista de páginas (vetor) e uma lista de segmentos. Os segmentos contém os processos e assumem o tamanho deles. Um segmento, portanto, pode ter várias páginas.

Ao apagar o segmento, as páginas correspondentes dele são liberadas para poderem ser ocupadas por outros processos.

A função de swap_out verifica se o processo foi modificado, caso o tenha sido, ela reescreve no arquivo.

A função de segunda chance percorre a lista de segmentos até encontrar aquele a ser removido, liberando esse espaço de memória. Sempre verifica o dirty bit e atualiza se ele foi modificado, para caso esse processo ser aquele a sair, ele poder ser reecopiado em disco.

### Semáforo

Para o semáforo, tem uma lista de semáforos, que é criada a medida que os processos são incluídos e inserida se não existir ainda na lista.

Para a fila de espera, cada semaforo possui uma dentro dele.

Em P, se o semáforo não estiver ocupado, ele é ocupado pelo processo. Caso contrário, o processo requisitor é colocado em espera.

Em V, ele é liberado apenas. O tratamento da fila de espera é feita pelo escalonador

### Round Robin

Tem uma lista de processos na CPU, no escalonador. O round robin verifica o quantum time e o tipo de instrução. Caso ainda tenha tempo de CPU após executar uma instrução, passa para próxima e executa até acabar seu tempo. Em caso de semáforos, se estiver ocupado e não puder usar o semáforo, ele é travado, até que o semáforo seja liberado. Se ele for liberado, o processo rodando é interrompido e passa para o que estava em espera pelo semáforo.

### Thread

Única thread para o Round Robin, que verifica uma *flag* que indica se pode travar ou não para criar o processo. Também verifica uma *flag* para imprimir, verificando com semáforo. 

### Interface

Foi feita no terminal. Mas na pasta Interface, há código com a *ncurses*, que vai ser arrumada para a parte 2.
