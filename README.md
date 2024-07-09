#  Trabalho Final - Plataforma IoT para Monitoramento de Dados de Sensores

## Introdução

O monitoramento de sistemas utilizando dados de sensores é crucial para garantir a eficiência operacional, minimizar o tempo de inatividade e prever falhas. Nesta tarefa, você desenvolverá uma plataforma IoT que coleta, processa e visualiza dados de sensores de sistemas escolhidos por você.

## Visão geral da arquitetura do sistema
O sistema será composto pelos seguintes módulos:

1. **DataCollector (Coletor de Dados)**: Responsável por coletar dados de vários sensores e publicar esses dados em tópicos MQTT específicos.

2. **DataProcessor (Processador de Dados)**: Assina os tópicos MQTT, processa os dados dos sensores e gera alertas com base em certos critérios.

3. **MQTT Broker**: Facilita a comunicação entre o SensorMonitor e o DataProcessor.

4. **Banco de Dados Temporal**: Persiste todas as informações.

5. **Ferramenta de Visualização de Dados**: Visualiza os dados.


Este repostiório tem exemplos de código para implementação dos módulos em C++, assim como a utilização do mosquitto como MQTT Broker. Porém, os componentes (e linguagem de programação) a serem utilizados são de livre escolha dos alunos, incluindo a possibilidade de utilizar componentes SaaS para o broker MQTT, banco de dados e ferramenta de visualização.

## DataCollector (Coletor de Dados)

O DataCollector será responsável por coletar dados de sensores escolhidos pelos alunos, como:

- Sensores de temperatura e umidade para monitoramento ambiental
- Sensores de velocidade e vibração para monitoramento de máquinas industriais
- Sensores de corrente e tensão para monitoramento elétrico
- Sensores de GPS para rastreamento de localização
- Sensores de consumo de memória, CPU e disco para monitoramento de servidores

O **DataCollector** pode ser implementado como um código em um sistema embarcado, um agente em uma máquina PC, ou um aplicativo mobile, dependendo do tipo de sensor que será utilizado. Por exemplo, para sensores de temperatura e umidade, um microcontrolador como o ESP32 pode ser utilizado; para monitoramento de servidores, um agente executando em um PC seria apropriado; e para sensores de GPS, um aplicativo mobile pode ser desenvolvido para coletar e publicar os dados.

Vocês devem coletar dados de no mínimo dois sensores.

Cada leitura de sensor será publicada em um tópico MQTT específico. O tópico para cada sensor deve seguir o formato:

```
/sensors/<id_da_maquina>/<id_do_sensor>
```
onde:

- `id_da_maquina` é um identificador único da máquina. Você pode usar o UUID (Universally Unique Identifier) da máquina como um identificador único. O UUID é um identificador padrão para recursos em um sistema de computação, existem bibliotecas capazes de gerá-los.
- `id_do_sensor` é o identificador do sensor que está sendo monitorado, por exemplo, `cpu_temperature`.

O módulo DataCollector deve ser capaz de ajustar a frequência com  cada sensor é lido e publicado. Esta frequência pode ser configurada via linha de comando ou por meio de um arquivo de configurações. 

O módulo deve ser projetado de maneira que a leitura e publicação de cada sensor seja realizada em uma tarefa distinta. Uma tarefa pode ser implementada como uma thread, um processo, ou utilizando um modelo de programação assíncrona. O objetivo aqui é que cada sensor possa ser lido e publicado de forma independente, sem bloquear a leitura e publicação de outros sensores.

### Formato da Mensagem

Cada leitura do sensor deve ser publicada como uma mensagem JSON, contendo um timestamp e o valor do sensor. Aqui está um exemplo do formato da mensagem:

```json
{
    "timestamp": "<timestamp>",
    "value": <sensor_value>
}
```

onde:

- `timestamp` é o momento em que a leitura do sensor foi realizada. Este deve estar no formato ISO 8601 e em tempo UTC. Exemplo: 2023-06-01T15:30:00Z.
- `value` é o valor da leitura do sensor. O tipo de dado (por exemplo, int, float) pode variar de acordo com o sensor.

Assegure-se de que a mensagem JSON esteja corretamente formatada e não contém erros de sintaxe antes de publicá-la no tópico MQTT.

## Mensagem Inicial do DataCollector

No início da execução, e a cada intervalo de tempo configurável, o **DataCollector** deve publicar uma mensagem inicial. Esta mensagem deve ser publicada no tópico `/sensor_monitors` e deve incluir as seguintes informações:

```json
{
    "machine_id": "id_da_maquina",
    "sensors": [
        {
            "sensor_id": "id_do_sensor",
            "data_type": "tipo_do_dado",
            "data_interval": periodicidade
        },
        ...
    ]
}
```

onde:

- `machine_id` é o identificador único da máquina
- `sensors` é uma lista dos sensores que serão monitorados. Para cada sensor, deve-se fornecer:
  - `sensor_id`: o nome do sensor que está sendo monitorado
  - `data_type`: o tipo de dado da leitura do sensor (por exemplo, int, float)
  -  `data_interval`: periocidade do envio dos dados (em milissegundos)

Este processo de envio periódico da mensagem inicial ajuda a garantir que todos os componentes do sistema estejam cientes das estações de trabalho que estão sendo monitoradas e dos sensores que estão ativos. A frequência com que essa mensagem inicial é enviada pode ser configurada via linha de comando ou por meio de um arquivo de configurações.

## DataProcessor (Processador de Dados)

O segundo módulo que você irá desenvolver será o **DataProcessor**. Este módulo será responsável por persistir e processar os dados dos sensores monitorados. 

O DataProcessor deve se inscrever no tópico `/sensor_monitors` e, para cada nova mensagem que indica uma nova máquina sendo monitorada, ele deve se inscrever nos tópicos correspondentes para cada sensor da máquina.

Para cada nova mensagem recebida, este módulo deve persistir a métrica recebida no banco de dados de séries temporais escolhido. As mensagens recebidas em cada tópico devem ser persistidas `metric path`s baseados na  `machine-id` e `sensor-id`. Isto é, o `metric-path`a ser usado deve ser `machine-id`.`sendor-id`.
  
Além disso, o dataProcessor também irá realizar dois tipos de processamento para cada nova mensagem de dados de um sensor:

1. **Alarme de Inatividade:** O DataProcessor deve gerar um alarme sempre que um dado de um sensor não for enviado por dez períodos de tempo previstos. Este é um indicador de que algo pode estar errado com o sensor ou com a máquina que está sendo monitorada.
2. **Processamento Personalizado:** Vocês devem definir um segundo tipo de processamento para as leituras dos sensores. Isso pode ser qualquer tipo de análise ou cálculo baseado nos dados do sensor. Algumas ideias podem incluir cálculos de média móvel, detecção de outliers ou análise de tendências.

Os alarmes gerados pelo DataProcessor devem ser persistidos no banco de dados de series temporais usando uma `metric path` do tipo `machine-id`.`alarms`.`alarm-type`.  Toda vez que um alarme for detectado, o valor 1, deve ser enviado ao banco de dados de series temporais.
 
Para o alarme de inatividade, por exemplo, o nome pode ser `inactive`.

Ao projetar e implementar o módulo DataProcessor, lembre-se de que ele precisa ser capaz de processar dados de múltiplas máquinas e sensores simultaneamente, de modo a não perder ou atrasar a leitura de mensagens de qualquer tópico. Isso pode exigir o uso de técnicas de programação concorrente ou assíncrona.

## Banco de Dados e Ferramenta de Visualização
Os alunos podem escolher as ferramentas de banco de dados e visualização que preferirem, podendo optar por soluções SaaS.

## Implementação e Flexibilidade
Os alunos têm total liberdade para escolher os componentes e tecnologias para cada módulo, incluindo a linguagem de programação, ferramentas de banco de dados e serviços de visualização. O único requisito é que a comunicação entre os módulos ocorra via MQTT.

Este repositório inclui exemplos de implementação dos módulos em C++ e a utilização do Mosquitto como MQTT Broker, mas os alunos podem utilizar outras linguagens e softwares conforme preferirem.
