#include "mqtt/async_client.h"

const std::string SERVER_ADDRESS("tcp://localhost:1883");
const std::string CLIENT_ID("ExampleSubscriber");
const std::string TOPIC("test/topic");

class callback : public virtual mqtt::callback {
public:
    virtual void message_arrived(mqtt::const_message_ptr msg) override {
        std::cout << "Mensagem recebida: " << msg->to_string() << std::endl;
    }
};

int main(int argc, char* argv[]) {
    mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);
    callback cb;
    client.set_callback(cb);

    // Conectando ao broker
    mqtt::connect_options connOpts;
    mqtt::token_ptr conntok = client.connect(connOpts);
    conntok->wait();

    // Assinando ao tópico
    client.subscribe(TOPIC, 1);

    // Esperar por mensagens
    std::this_thread::sleep_for(std::chrono::seconds(60));

    // Desconectando
    client.disconnect()->wait();

    return 0;
}
