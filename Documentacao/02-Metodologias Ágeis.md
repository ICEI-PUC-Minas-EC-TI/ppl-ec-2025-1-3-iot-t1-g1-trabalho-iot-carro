## Gerenciamento de Projeto


# Metodologia

A equipe adotou uma abordagem baseada em metodologias ágeis, utilizando o Scrum como estrutura principal para organizar o trabalho e atacar o problema proposto. O desenvolvimento do projeto ocorreu de forma incremental e colaborativa, com reuniões frequentes realizadas tanto em salas de aula quanto na biblioteca da faculdade. Esses ambientes favoreceram a comunicação eficiente, a troca de ideias e a rápida resolução de dúvidas.

### Divisão de Papéis

Apresente a divisão de papéis entre os membros do grupo.

A organização do time seguiu a estrutura do Scrum, sendo distribuída da seguinte forma:

Scrum Master: Bernardo Rodrigues Pereira

Equipe de Desenvolvimento: Alexandre Versiani Raposo, Bernardo Rodrigues Pereira, Eduardo Utsch Gonçalves, Gabriel Mamede Albino, Marcos Vinicius dos Santos Pereira, Pedro Gonçalves Contantino

O Scrum Master ficou responsável por facilitar as reuniões, remover impedimentos e garantir a aplicação das práticas ágeis. Os demais membros atuaram no desenvolvimento, testes e integração das funcionalidades do sistema.


### Processo

O processo de desenvolvimento foi dividido em sprints curtas, cada uma com objetivos bem definidos e entregas incrementais. A equipe realizou reuniões rápidas (dailies) para alinhar tarefas e identificar possíveis dificuldades.

O desenvolvimento técnico seguiu uma abordagem progressiva:
Inicialmente, os sensores foram testados individualmente com LEDs para validar seu funcionamento básico.
Em seguida, foi estabelecida a conexão com o broker MQTT, inicialmente sem os sensores.
Posteriormente, os sensores e atuadores foram integrados ao sistema de forma incremental. A cada etapa, um novo sensor era adicionado e validado, repetindo o processo até que todos estivessem integrados e funcionando corretamente.
Por fim, foram realizados testes finais para garantir o funcionamento do sistema como um todo.
 

### Ferramentas

As ferramentas utilizadas no desenvolvimento do projeto foram:

Arduino IDE: Programação e upload do firmware nos microcontroladores.
PyCharm: Ambiente de desenvolvimento para scripts em Python.
MQTT Box: Testes e monitoramento do tráfego de mensagens MQTT.
IoT MQTT Panel: Visualização e interação remota com sensores e atuadores via protocolo MQTT.
MySQL: Armazenamento e gerenciamento dos dados coletados.
GitHub: Gestão do código-fonte, controle de versões e colaboração da equipe.
