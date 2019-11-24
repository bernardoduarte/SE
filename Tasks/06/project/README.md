# Projeto Office Hot Desk

## Descrição

Baseado na sugestão dada pelo professor através deste [link](https://www.thingsquare.com/blog/articles/hot-desk-usage-tracking/), o projeto será de um sistema de detecção de utilização das mesas de um espaço de coworking. O sistema utilizará um arduino com detector de presença para cada mesa, que se comunica através de um módulo de rádio frequência as informações capturadas pelo detector para um arduino central que armazena as informações recebidas.

## Componentes

1. 3x Arduino UNO R3
2. 2x Sensor de Movimento PIR DYP-ME003
3. 3x Transmissor RF MX-FS-03V
4. 3x Receptor RF MX-05V
5. 1x Módulo Cartão Micro SD
6. 3x Mini-Protoboard 170 pontos

## Problematização Professor

Quebra da hipótese de que todo o nó (mesa) tenha acesso ao nó central (armazenador). Para resolver isso, criar um esquema de roteamento entre os nós para atingir o central. Exemplo básico: M1 -> M2 -> Central; ou seja, a mesa 1 se comunica com a central pela mesa 2.
