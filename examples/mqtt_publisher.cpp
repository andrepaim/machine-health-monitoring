#include "mqtt/async_client.h"

const std::string SERVER_ADDRESS("tcp://localhost:1883");
const std::string CLIENT_ID("ExampleClient");

int main(int argc, char* argv[]) {
    mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);

    // Conectando ao broker
    mqtt::connect_options connOpts;
    mqtt::token_ptr conntok = client.connect(connOpts);

    // Esperando pela conexão
    conntok->wait();

    // Publicando uma mensagem
    mqtt::message_ptr pubmsg = mqtt::make_message("test/topic", "Hello, MQTT!");
    client.publish(pubmsg)->wait_for(std::chrono::seconds(10));

    // Desconectando
    client.disconnect()->wait();

    return 0;
}
