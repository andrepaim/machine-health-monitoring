#  Trabalho Final - Monitoramento da Saúde de Máquinas

## Introdução

O monitoramento da saúde de máquinas em ambientes industriais é crucial para garantir a eficiência operacional, minimizar o tempo de inatividade e prever falhas de equipamentos. Nesta tarefa, você desenvolverá um sistema de monitoramento de saúde de máquinas que coleta, processa e visualiza dados de sensores de várias estações de trabalho.

## Visão geral da arquitetura do sistema
O sistema será composto pelos seguintes módulos:

1. **SensorMonitor (Monitor de Sensores)**: Este módulo será responsável por coletar dados de vários sensores de uma estação de trabalho, tais como temperatura da CPU, carga do sistema, porcentagem de memória livre e porcentagem de espaço em disco livre. Ele publicará esses dados em tópicos MQTT específicos.

2. **DataProcessor (Processador de Dados)**: Este módulo será responsável por assinar os tópicos MQTT, processar os dados dos sensores (como análise de tendências, detecção de anomalias, análise da taxa de variação, etc.) e gerar alertas com base em certos critérios. 

3. **MQTT Broker**: Este é o corretor MQTT que facilitará a comunicação entre o SensorMonitor e o DataProcessor. 

4. **Banco de Dados**: Este módulo será responsável por persistir todas as informações. Iremos utilizar o MongoDB.

5. **Ferramenta de Visualização de Dados**: Este módulo será responsável pela visualização dos dados. Vocês devem explorar e definir uma ferramenta para realizar a visualização.

Esses módulos trabalharão em conjunto para fornecer uma visão holística da saúde das máquinas, permitindo intervenções oportunas e manutenção proativa.

Este repostiório tem exemplos de código para implementação dos módulos em C++, assim como a utilização do mosquitto como MQTT Broker e o MongoDB como banco de dados. Porém, é apenas uma sugestão. Voces podem utilizar outras linguagens e softwares.


## SensorMonitor (Monitor de Sensores)

O primeiro módulo que você irá desenvolver será o **SensorMonitor**. Este módulo será responsável por coletar dados de sensores  de uma estação de trabalho. Vocês devem definir quais sensores serão monitorados, exemplos de sensores são:

- Porcentagem de memória utilizada
- Porcentagem de CPU utilizada
- Porcentagem de disco utilizado
- Ou qualquer outro sensor que voces escolham

Vocês devem coletar no mínimo dois sensores (sugestão de um colega seus durante a aula).

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

O segundo módulo que você irá desenvolver será o **DataProcessor**. Este módulo será responsável por persistir e processar os dados dos sensores monitorados. 

O DataProcessor deve se inscrever no tópico `/sensor_monitors` e, para cada nova mensagem que indica uma nova máquina sendo monitorada, ele deve se inscrever nos tópicos correspondentes para cada sensor da máquina.

Para cada nova mensagem recebida, este módulo deve persistir a mensagem no MongoDB (banco de dados NoSQL). As mensagens recebidas em cada tópico devem ser persistidas em coleções baseadas no `sensor_id` (o nome da collection será o `sensor_id`. O `machine_id` deve ser anexado a cada documento como um valor de string. Resumindo, cada documento deve ter três campos: 
- `machine_id` : string
- `timestamp` : formato de data e hora nativo do MongoDB
- `value`: baseado no campo `data_type`
  
Além disso, o dataProcessor também irá realizar dois tipos de processamento para cada nova mensagem de dados de um sensor:

1. **Alarme de Inatividade:** O DataProcessor deve gerar um alarme sempre que um dado de um sensor não for enviado por dez períodos de tempo previstos. Este é um indicador de que algo pode estar errado com o sensor ou com a máquina que está sendo monitorada.
2. **Processamento Personalizado:** Vocês devem definir um segundo tipo de processamento para as leituras dos sensores. Isso pode ser qualquer tipo de análise ou cálculo baseado nos dados do sensor. Algumas ideias podem incluir cálculos de média móvel, detecção de outliers ou análise de tendências.

Os alarmes gerados pelo DataProcessor devem ser persistidos em uma coleção do MongoDb denominada`alarms`. Cada alarme deve ser um documento com os seguintes campos

- `machine_id` -  string contendo identificador único da máquina
- `sensor_id` - string com nome do sensor que está sendo monitorado
- `description` - string com uma descrição textual do alarme. 
 
Para o alarme de inatividade, por exemplo, a descrição pode ser "Sensor inativo por dez períodos de tempo previstos".

Ao projetar e implementar o módulo DataProcessor, lembre-se de que ele precisa ser capaz de processar dados de múltiplas máquinas e sensores simultaneamente, de modo a não perder ou atrasar a leitura de mensagens de qualquer tópico. Isso pode exigir o uso de técnicas de programação concorrente ou assíncrona.

## Banco de Dados

Como mencionado, o repositório utiliza o MongoDB como banco de dados. Lembre-se que isso é apenas uma sugestão, vocês podem utilizar outro banco de dados a sua escolha.

O repositório já tem uma extensão do Mongo para Visual Studio Code pre definida no devcontainer.

Para acessar o Mongo e visualizar os dados persistidos, realize os seguintes passos:

1. Clique no ícone de uma folha na barra lateral esquerda
2. Clique em `Add connection`
3. Clique em `Open Form` em `Advanced Connection Settings`
4. Modifique o valor do campo `Hostname` para `db` e clique em `Connect`

Se os passos forem executados com êxito, voce terá acesso a uma árvore de navegação das coleções do banco na aba lateral esquerda.
