# BattleType

BattleType é um projeto simples de batalha de digitação multiplayer desenvolvido como trabalho acadêmico por [Emílio B. Pedrollo] e [Fábio A. Pedroso].

## Instalando Depêndencias

Primeiro é necessário habilitar as fonte de software 'universe': 
```sh
# add-apt-repository universe && apt-get update
```

Agora iremos instalar todas nossas dependências:

```sh
# apt-get install liballegro5.0 liballegro5-dev libenet7 libenet-dev git
```

## Fazendo Checkout

Execute as seguintes linhas para fazer uma cópia local dos fontes no seu computador 

```sh
$ git clone https://github.com/emiliopedrollo/battle-type.git && cd battle-type/
```
## Compilando
Há várias formas de compilar este projeto, aqui descrevo duas delas que provavelmente são as mais simples que podemos fazer:
#### Por linha de comando
Um único comendo nos gera o executável:
```sh
$ cmake .
$ make
```

## Executando o jogo

Após compilado o executável do jogo estará em `output/battle_type` você só precisa chama-lo pelo terminal ou com um duplo clique para abrir o jogo

## Especificações do trabalho

### Chuva de palavras

A proposta é implementar um jogo baseado no jogo Z-Type (http://zty.pe/), conforme
mostra a figura abaixo:

O jogo é composto por um retângulo, no qual o jogador encontra-se fixado na parte
inferior e palavras “caem” em direção ao mesmo. Trata-se de um jogo de nave e tiro
onde, para eliminar os inimigos, é preciso ser bom em digitação. Cada inimigo traz sua
palavra que precisa ser digitada para eliminá-lo. Assim que uma das palavras começa
a ser escrita, ela identifica o inimigo alvo, e essa palavra deve ser digitada até o final
para eliminar aquele inimigo e iniciar o ataque a outro. O inimigo selecionado deve ser
desenhado de forma a ser facilmente reconhecido. São mostradas somente as letras
que faltam da palavra do inimigo selecionado. Quando não há nenhum inimigo
selecionado e o jogador digita uma letra, é selecionado o inimigo cuja palavra inicia
com a letra selecionada que esteja mais próximo do jogador. Caso nenhuma das
palavras dos inimigos iniciem com a letra digitada, nenhum inimigo é selecionado e a
letra é considerada errada para fins de pontuação. É preciso eliminar todos os inimigos
antes que eles alcancem o jogador.

Os inimigos devem ter velocidades individualizadas, escolhidas de forma aleatória na
hora de sua criação. Para tornar a trajetória dos inimigos menos óbvia, sugere-se que
seja escolhida uma velocidade no eixo x na hora da criação do inimigo, e que essa
velocidade seja alterada em função de uma aceleração, que por sua vez é
proporcional à distância no eixo x entre o inimigo e o jogador (veja pseudocódigo
abaixo). A velocidade em y pode ser constante (mas individualizada).

Neste jogo não há comandos de movimento, o jogador fica parado todo o tempo. As
únicas teclas que funcionam são as letras. O mouse não funciona.

O jogo é composto por fases; a cada fase um certo número de palavras é escolhido
pelo jogo e elas vão descendo desde uma posição aleatória no topo da tela em
direção ao jogador. A fase termina quando todas as palavras da fase forem eliminadas
pelo jogador. Após uma breve pausa, o jogo inicia a fase seguinte. A cada nova fase,
devem aumentar os níveis de dificuldade (tamanho das palavras, número de palavras,
velocidade de queda das palavras, etc.).

O jogador ganha pontos para cada letra correta digitada, da seguinte forma: a primeira
letra vale 10 pontos, a segunda 11, a terceira 12 e assim por diante. Na segunda fase,
a primeira letra vale 20 pontos, a segunda 21, etc. Na terceira fase, a sequência
começa em 30 pontos. Uma letra errada desconta o número de pontos do início da
sequência da fase, e reinicia a sequência.

As palavras devem ser oriundas de um arquivo texto, fornecido pelo jogo. Esse
arquivo contém as palavras que o jogo irá utilizar para identificar os inimigos, no
formato de uma palavra por linha.

Como sugestão de implementação, recomenda-se separar as palavras em dois
vetores, um com palavras curtas e outro com palavras compridas. Quando uma
palavra é sorteada, realiza-se o sorteio em duas etapas: inicialmente escolhe-se de
que vetor será escolhida a palavra e depois escolhe-se uma palavra qualquer desse
vetor. Na escolha do vetor, dá-se um peso diferente dependendo da fase em que o
jogo se encontra. Por exemplo, na primeira fase, sorteia-se um número entre 0 e 100 e
se for maior que 95 escolhe-se uma palavra do vetor difícil, senão do fácil. A cada
fase, diminui-se esse limite, aumentando a probabilidade de o vetor de palavras
difíceis ser escolhido.

Deve haver uma forma de pausar e de retornar ao jogo.

O jogador perde a vida quando alguma das palavras alcançar a base do retângulo (ou
o jogador). Caso o jogador tenha pontos ganhos suficientes, o jogo lhe apresenta a
possibilidade de compra de uma nova vida. A primeira vida custa 100 pontos, a
segunda vida custa 200, e vai dobrando de preço. O jogo termina quando o jogador
perde a vida e não tem pontos suficientes para comprar a próxima ou não quer
comprar a próxima.

Ao final do jogo, deve ser mostrado o histórico com as dez maiores pontuações já
obtidas e a pontuação do jogo recém terminado, destacada, e na ordem de maior para
menor. As dez maiores pontuações devem ser mantidas em um arquivo e devem ser
apresentadas juntamente com o nome do jogador (informação inserida no início de
cada jogo).

[Emílio B. Pedrollo]: <https://github.com/emiliopedrollo>
[Fábio A. Pedroso]: <https://github.com/fapedroso>

