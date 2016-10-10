# BattleType

BattleType é um projeto simples de batalha de digitação multiplayer desenvolvido como trabalho acadêmico por [Emílio B. Pedrollo] e [Fábio A. Predroso].

## Instalando Depêndencias

Primeiro é necessário habilitar as fonte de software 'universe': 
```sh
# add-apt-repository universe
# apt-get update
```

Agora iremos instalar todas nossas dependências:

```sh
# apt-get install liballegro5.0 liballegro5-dev libenet7 libenet-dev git
```

##### Opcional

Instalar o netbeans:
```sh
# apt-get install netbeans
```

Não se esqueça de habilitar o plugin para desenvolvimento em C/C++ no Netbeans caso escolha esta IDE para desenvolvimento.

## Fazendo Checkout

Execute as seguintes linhas para fazer uma cópia local dos fontes no seu computador 

```sh
$ mkdir ~/BattleType && cd ~/BattleType
$ git clone https://github.com/emiliopedrollo/battle-type.git .
```
## Compilando
Há várias formas de compilar este projeto, aqui descrevo duas delas que provavelmente são as mais simples que podemos fazer:
#### Por linha de comando
Um único comendo nos gera o executável:
```sh
$ make
```
#### Pelo Netbeans
Aqui você pode que abrir a pasta projeto como um projeto nativo e simplesmente clicar em Executar (ou atraés do atalho F6).

## Executando o jogo

Após compilado o executável do jogo estará em `~/BattleType/dist/Debug/GNU-Linux/battletype` você só precisa chama-lo pelo terminal ou com um duplo clique para abrir o jogo

[Emílio B. Pedrollo]: <https://github.com/emiliopedrollo>
[Fábio A. Predroso]: <https://github.com/fapedroso>

