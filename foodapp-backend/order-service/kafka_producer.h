#ifndef KAFKA_PRODUCER_H
#define KAFKA_PRODUCER_H

#include <librdkafka/rdkafka.h>
#include <iostream>
#include <string>

class KafkaProducer {
private:
    rd_kafka_t *rk;          // Producer instance
    rd_kafka_conf_t *conf;   // Configuration object
    std::string topic_name;

public:
    KafkaProducer(const std::string& brokers, const std::string& topic) : topic_name(topic) {
        char errstr[512];
        conf = rd_kafka_conf_new();

        // 1. Connection & SSL Settings
        rd_kafka_conf_set(conf, "bootstrap.servers", brokers.c_str(), errstr, sizeof(errstr));
        rd_kafka_conf_set(conf, "security.protocol", "ssl", errstr, sizeof(errstr));
        rd_kafka_conf_set(conf, "ssl.ca.location", "ssl/ca.pem", errstr, sizeof(errstr));
        rd_kafka_conf_set(conf, "ssl.certificate.location", "ssl/service.cert", errstr, sizeof(errstr));
        rd_kafka_conf_set(conf, "ssl.key.location", "ssl/service.key", errstr, sizeof(errstr));

        // 2. Create Producer Instance
        rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
        if (!rk) {
            std::cerr << "❌ Failed to create producer: " << errstr << std::endl;
        }
    }

    // Function to send message to Kafka
    void sendMessage(const std::string& message) {
        rd_kafka_resp_err_t err;

        err = rd_kafka_producev(
            rk,
            RD_KAFKA_V_TOPIC(topic_name.c_str()),
            RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
            RD_KAFKA_V_VALUE((void*)message.c_str(), message.size()),
            RD_KAFKA_V_END);

        if (err) {
            std::cerr << "❌ Failed to produce to topic " << topic_name << ": " << rd_kafka_err2str(err) << std::endl;
        } else {
            std::cout << "🚀 [KAFKA PRODUCED]: " << message << std::endl;
        }

        // Poll to handle delivery reports
        rd_kafka_poll(rk, 0);
    }

    ~KafkaProducer() {
        std::cout << "Shutting down Kafka Producer..." << std::endl;
        rd_kafka_flush(rk, 10000); // Wait for max 10s to deliver pending messages
        rd_kafka_destroy(rk);
    }
};

#endif