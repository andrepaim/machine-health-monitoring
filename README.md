# Trabalho Final - Plataforma IoT para Monitoramento de Dados de Sensores

## Introdução

O monitoramento de sistemas utilizando dados de sensores é crucial para garantir a eficiência operacional, minimizar o tempo de inatividade e prever falhas. Nesta tarefa, vocês desenvolverão uma plataforma IoT que coleta, processa e visualiza dados de sensores de sistemas escolhidos por vocês.

## 1. Visão Geral da Arquitetura do Sistema

O sistema será composto pelos seguintes módulos:

1. **DataCollector (Coletor de Dados)**: Responsável por coletar dados de vários sensores e publicar esses dados em tópicos MQTT específicos.
2. **DataProcessor (Processador de Dados)**: Assina os tópicos MQTT, processa os dados dos sensores e gera alertas com base em certos critérios.
3. **MQTT Broker**: Facilita a comunicação entre o DataCollector e o DataProcessor.
4. **Banco de Dados Temporal**: Persiste todas as informações.
5. **Ferramenta de Visualização de Dados**: Visualiza os dados.

Este repositório contém exemplos de código para implementação dos módulos em C++, assim como a utilização do Mosquitto como MQTT Broker. Entretanto, os componentes (e linguagem de programação) a serem utilizados são de livre escolha dos alunos, incluindo a possibilidade de utilizar componentes SaaS para o broker MQTT, banco de dados e ferramenta de visualização.

## 2. DataCollector (Coletor de Dados)

O **DataCollector** será responsável por coletar dados de sensores escolhidos pelos alunos, como:

- Sensores de temperatura e umidade para monitoramento ambiental
- Sensores de velocidade e vibração para monitoramento de máquinas industriais
- Sensores de corrente e tensão para monitoramento elétrico
- Sensores de GPS para rastreamento de localização

### Configuração e Publicação de Dados

O **DataCollector** pode ser implementado como um código em um sistema embarcado, um agente em uma máquina PC, ou um aplicativo mobile, dependendo do tipo de sensor que será utilizado. Vocês devem coletar dados de no mínimo dois sensores.

Cada leitura de sensor será publicada em um tópico MQTT específico. O tópico para cada sensor deve seguir o formato:

```
/sensors/<id_da_maquina>/<id_do_sensor>
```

#### Formato da Mensagem

Cada leitura do sensor deve ser publicada como uma mensagem JSON, contendo um timestamp e o valor do sensor. Exemplo:

```json
{
    "timestamp": "2024-12-10T15:30:00Z",
    "value": 23.5
}
```

- `timestamp`: Momento da leitura, no formato ISO 8601 em UTC.
- `value`: Valor da leitura do sensor, podendo ser `int`, `float`, etc.

#### Mensagem Inicial do DataCollector

No início da execução e a cada intervalo de tempo configurável, o **DataCollector** deve publicar uma mensagem inicial no tópico `/sensor_monitors`:

```json
{
    "machine_id": "id_da_maquina",
    "sensors": [
        {
            "sensor_id": "id_do_sensor",
            "data_type": "tipo_do_dado",
            "data_interval": periodicidade
        }
    ]
}
```

Esta mensagem informa quais sensores estão sendo monitorados e suas respectivas periodicidades.

## 3. DataProcessor (Processador de Dados)

O **DataProcessor** será responsável por persistir e processar os dados recebidos. Ele deve:

1. Se inscrever no tópico `/sensor_monitors` e nos tópicos correspondentes para cada sensor monitorado.
2. Persistir os dados em um banco de dados temporal, utilizando um `metric path` baseado em `machine-id` e `sensor-id`.
3. Implementar os seguintes processamentos:
   
   - **Alarme de Inatividade**: Gerar um alarme se um sensor não enviar dados por dez periódos previstos.
   - **Processamento Personalizado**: Definir um tipo de processamento adicional, como média móvel, detecção de outliers ou análise de tendências.

Os alarmes gerados devem ser persistidos no banco de dados em um `metric path` do tipo `machine-id.alarms.alarm-type`. Por exemplo:

```plaintext
machine-id.alarms.inactive
```

## 4. Banco de Dados e Ferramenta de Visualização

Os alunos devem propor um banco de dados e uma ferramenta de visualização para os dados. Vocês podem optar por soluções SaaS ou ferramentas locais. A solução deve ser capaz de exibir os dados coletados e processados de forma clara e intuitiva.

## 5. Proposta de Sistema

A primeira entrega do trabalho será a proposta de um sistema baseado na arquitetura descrita. Nesta proposta, vocês devem:

1. **Definir os Sensores e o DataCollector**: Propor um tipo de sensor inteligente e descrever como os dados serão coletados. Podem ser utilizados:
   - Microcontroladores conectados a sensores (e.g., Arduino, ESP32).
   - Sensores virtuais, com dados capturados via APIs publicas.
   - Aplicativos mobile para coleta de dados sensoriais, como sensores de movimento, GPS ou de condições ambientais embutidos em smartphones.
2. **Processamento de Dados**: Descrever o tipo de processamento que será realizado nos dados, incluindo os critérios de alerta.
3. **Banco de Dados e Visualização**: Propor as ferramentas que serão utilizadas para persistência e visualização dos alarmes, dados crus de sensores e processados.


### Observações Importantes

1. A solução final **não** deve ser implementada utilizando o GitHub Codespaces. O repositório do projeto deve servir apenas como referência.
2. Não será aceita a coleta de dados provenientes de PCs (como consumo de memória, CPU, etc.), pois esse tema já foi abordado em trabalhos de semestres anteriores. Certifiquem-se de inovar ao selecionar os tipos de sensores e fontes de dados para este projeto.

## 6. Implementação e Flexibilidade

Vocês têm total liberdade para escolher os componentes e tecnologias para cada módulo, desde que a comunicação entre eles ocorra via MQTT. Se precisarem de suporte, consultem os exemplos no repositório ou procurem materiais adicionais para orientar suas escolhas.

Boa sorte no desenvolvimento da plataforma IoT!


