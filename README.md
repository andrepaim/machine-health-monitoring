#  Trabalho Final - Monitoramento da Saúde de Máquinas

## Introdução

O monitoramento da saúde de máquinas em ambientes industriais é crucial para garantir a eficiência operacional, minimizar o tempo de inatividade e prever falhas de equipamentos. Nesta tarefa, você desenvolverá um sistema de monitoramento de saúde de máquinas que coleta, processa e visualiza dados de sensores de várias estações de trabalho.

## Visão geral da arquitetura do sistema
O sistema será composto pelos seguintes módulos:

1. **SensorMonitor (Monitor de Sensores)**: Este módulo será responsável por coletar dados de vários sensores de uma estação de trabalho, incluindo temperatura da CPU, carga do sistema, porcentagem de memória livre e porcentagem de espaço em disco livre. Ele publicará esses dados em tópicos MQTT específicos.

2. **DataProcessor (Processador de Dados)**: Este módulo será responsável por assinar os tópicos MQTT, processar os dados dos sensores (como análise de tendências, detecção de anomalias, análise da taxa de variação, etc.) e gerar alertas com base em certos critérios. Os resultados serão publicados em tópicos MQTT específicos.

3. **MQTT Broker**: Este é o corretor MQTT que facilitará a comunicação entre o SensorMonitor e o DataProcessor.

4. **Banco de Dados**: Este módulo será responsável por persistir todas as informações. Iremos utilizar o MongoDB.

5. **Ferramenta de Visualização de Dados**: Este módulo será responsável pela visualização dos dados. Iremos usar o Grafana.

Esses módulos trabalharão em conjunto para fornecer uma visão holística da saúde das máquinas, permitindo intervenções oportunas e manutenção proativa.


## SensorMonitor (Monitor de Sensores)

O primeiro módulo que você irá desenvolver será o **SensorMonitor**. Este módulo será responsável por coletar dados dos seguintes sensores em uma estação de trabalho:

1. Porcentagem de memória utilizada
2. Porcentagem de CPU utilizada
3. Porcentagem de disco utilizado
4. Mais dois sensores à escolha dos alunos. Estes podem ser quaisquer sensores disponíveis na estação de trabalho, tais como temperatura da CPU, carga do sistema, tráfego de rede, entre outros.

Cada leitura de sensor será publicada em um tópico MQTT específico. O tópico para cada sensor deve seguir o formato:

```
/sensors/<id_da_maquina>/<id_do_sensor>
```
onde:

- `id_da_maquina` é um identificador único da máquina. Você pode usar o UUID (Universally Unique Identifier) da máquina como um identificador único. O UUID é um identificador padrão para recursos em um sistema de computação e pode ser obtido em uma máquina Linux usando o comando `cat /etc/machine-id`.
- `id_do_sensor` é o identificador do sensor que está sendo monitorado, por exemplo, `cpu_temperature`.

O módulo SensorMonitor deve ser capaz de ajustar a frequência com 
 cada sensor é lido e publicado. Esta frequência pode ser configurada via linha de comando ou por meio de um arquivo de configurações. 

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

## Mensagem Inicial do SensorMonitor

No início da execução, e a cada intervalo de tempo configurável, o **SensorMonitor** deve publicar uma mensagem inicial. Esta mensagem deve ser publicada no tópico `/sensor_monitors` e deve incluir as seguintes informações:

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

O segundo módulo que você irá desenvolver será o **DataProcessor**. Este módulo será responsável por processar os dados dos sensores monitorados. O DataProcessor deve se inscrever no tópico `/sensor_monitors` e, para cada nova mensagem que indica uma nova máquina sendo monitorada, ele deve se inscrever nos tópicos correspondentes para cada sensor da máquina.

O DataProcessor irá realizar dois tipos de processamento para cada nova mensagem de dados de um sensor:

1. **Alarme de Inatividade:** O DataProcessor deve gerar um alarme sempre que um dado de um sensor não for enviado por dois períodos de tempo previstos. Este é um indicador de que algo pode estar errado com o sensor ou com a máquina que está sendo monitorada.
2. **Processamento Personalizado:** Vocês devem definir um segundo tipo de processamento para as leituras dos sensores. Isso pode ser qualquer tipo de análise ou cálculo baseado nos dados do sensor. Algumas ideias podem incluir cálculos de média móvel, detecção de outliers ou análise de tendências.

Os alarmes gerados pelo DataProcessor devem ser publicados no tópico `/alarms`. Cada alarme deve ser uma mensagem JSON com o seguinte formato:

```json
{
    "machine_id": "id_da_maquina",
    "sensor_id": "id_do_sensor",
    "description": "descrição do alarme"
}
```
onde:

- `machine_id` é o identificador único da máquina
- `sensor_id` é o nome do sensor que está sendo monitorado
- `description` é uma descrição textual do alarme. 
 
Para o alarme de inatividade, por exemplo, a descrição pode ser "Sensor inativo por dois períodos de tempo previstos".

Ao projetar e implementar o módulo DataProcessor, lembre-se de que ele precisa ser capaz de processar dados de múltiplas máquinas e sensores simultaneamente, de modo a não perder ou atrasar a leitura de mensagens de qualquer tópico. Isso pode exigir o uso de técnicas de programação concorrente ou assíncrona.
