
# Materiais

Os materiais utilizados no projeto foram:

1 ESP32
2 Sensores ultrassônicos
1 Sensor de temperatura NTC
1 Sensor de luminosidade LDR
2 LEDs vermelhos
2 LEDs azuis
2 LEDs amarelos
1 Ponte H
2 Motores DC
1 Chassi
2 Rodas

# Desenvolvimento

O projeto foi inicialmente dividido em etapas para facilitar a integração entre hardware, software embarcado e o aplicativo de controle.
Foram definidos os materiais necessários e as funcionalidades principais que o sistema deveria apresentar.
Todo o processo de desenvolvimento foi pensado para garantir que os diferentes componentes (hardware, código embarcado e aplicativo) funcionassem de maneira coordenada.
Os testes iniciais de cada componente foram feitos separadamente para garantir o funcionamento individual antes da integração total.
A comunicação entre o aplicativo e o hardware foi realizada utilizando o protocolo MQTT.
O ESP32 foi configurado para conectar-se à rede Wi-Fi e ao broker MQTT, permitindo a troca de mensagens (comandos e dados de sensores) de forma eficiente e em tempo real.
Após a montagem e programação, foram feitos testes integrados para validar o funcionamento conjunto de todo o sistema.
Ajustes foram realizados conforme necessário para garantir a confiabilidade e a interação correta entre aplicativo, sensores, atuadores e o controle remoto.


## Desenvolvimento do Aplicativo

### Interface

O aplicativo foi desenvolvido utilizando o IoT MQTT Panel. As telas foram criadas para permitir o controle dos motores (movimentação do carrinho), visualização dos dados dos sensores (temperatura, luminosidade e distância) e acionamento dos LEDs indicadores. Cada função foi vinculada a botões, sliders e indicadores visuais da interface do painel.

### Código

Como o IoT MQTT Panel é uma ferramenta de configuração visual, o desenvolvimento do “código” do aplicativo consistiu em configurar corretamente os tópicos MQTT para cada botão e indicador. Os comandos de controle (como direção do carrinho e acionamento dos LEDs) utilizam tópicos de publish, enquanto os dados dos sensores são recebidos via tópicos de subscribe.

## Desenvolvimento do Hardware

### Montagem

A montagem do projeto iniciou-se com a fixação dos motores DC ao chassi, seguidos pela instalação das rodas. O ESP32 foi fixado em um local central para facilitar as conexões. Os sensores ultrassônicos foram posicionados na parte frontal e traseira do carrinho para detecção de obstáculos. O sensor NTC foi instalado para captar a temperatura da ponte H e o LDR, em local exposto à luz. Os LEDs foram distribuídos estrategicamente para sinalização visual, representando os farois, luz de freio e setas. Todos os componentes foram conectados ao ESP32, respeitando as especificações de alimentação e controle.

### Desenvolvimento do Código

O código foi desenvolvido em C++ utilizando a IDE do Arduino. Inicialmente, foram implementadas rotinas individuais para testar cada sensor e atuador. A comunicação MQTT foi adicionada, permitindo que o ESP32 publicasse os dados dos sensores e recebesse comandos do aplicativo. Em seguida, integrou-se o controle dos motores via ponte H, o acionamento dos LEDs e a leitura dos sensores. O código foi modularizado, facilitando ajustes e manutenção. 

## Comunicação entre App e Hardware

A comunicação entre o aplicativo IoT MQTT Panel e o hardware foi estabelecida utilizando o protocolo MQTT. O ESP32 foi programado para se conectar a uma rede Wi-Fi e a um broker MQTT. Os dados dos sensores são publicados em tópicos específicos, enquanto os comandos do aplicativo (controle dos motores, LEDs etc.) são recebidos por meio de tópicos de subscribe. Essa abordagem possibilitou o controle em tempo real do carrinho e o monitoramento remoto dos dados do ambiente.
