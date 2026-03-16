#include <librdkafka/rdkafka.h>
#include <iostream>
#include <string>
#include <csignal>

using namespace std;

// Global flag to handle graceful shutdown
static bool run = true;
static void stop(int sig) { run = false; }

int main() {
    signal(SIGINT, stop); // Catch Ctrl+C

    char errstr[512];
    rd_kafka_conf_t *conf = rd_kafka_conf_new();

    // 1. Connection Settings (Points to Aiven Kafka)
    rd_kafka_conf_set(conf, "bootstrap.servers", "foodapp-kafka-broker-lpu-e79e.k.aivencloud.com:21836", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "security.protocol", "ssl", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "ssl.ca.location", "certificates/ca.pem", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "ssl.certificate.location", "certificates/service.cert", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "ssl.key.location", "certificates/service.key", errstr, sizeof(errstr));

    // 2. Consumer Group Settings
    // group.id allows Kafka to track which messages this service has already read
    rd_kafka_conf_set(conf, "group.id", "order_service_consumer_group", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "enable.auto.commit", "true", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "auto.offset.reset", "earliest", errstr, sizeof(errstr));

    // Create the Consumer instance
    rd_kafka_t *rk = rd_kafka_new(RD_KAFKA_CONSUMER, conf, errstr, sizeof(errstr));
    if (!rk) {
        cerr << "❌ Failed to create consumer: " << errstr << endl;
        return 1;
    }

    // 3. Subscribe to the 'user-events' topic (The Producer's Topic)
    rd_kafka_topic_partition_list_t *topics = rd_kafka_topic_partition_list_new(1);
    rd_kafka_topic_partition_list_add(topics, "user-events", RD_KAFKA_PARTITION_UA);
    rd_kafka_subscribe(rk, topics);

    cout << "🎧 Order Service is LIVE." << endl;
    cout << "📍 Subscribed to topic: 'user-events'" << endl;
    cout << "⌛ Waiting for new user registrations..." << endl;
    cout << "--------------------------------------------------" << endl;

    // 4. The Event Loop
    while (run) {
        rd_kafka_message_t *rkmessage = rd_kafka_consumer_poll(rk, 1000); // Poll every 1s
        
        if (rkmessage) {
            if (rkmessage->err == RD_KAFKA_RESP_ERR_NO_ERROR) {
                // Success: Message Received
                printf("\n🔔 [KAFKA EVENT]: %.*s\n", (int)rkmessage->len, (const char *)rkmessage->payload);
                
                // --- Phase 3 Business Logic ---
                cout << "🛒 ACTION: New user detected! Activating 50% 'WELCOME50' discount in DB..." << endl;
                cout << "📩 LOG: Welcome email event triggered for this user ID." << endl;
                cout << "--------------------------------------------------" << endl;
            } 
            else if (rkmessage->err == RD_KAFKA_RESP_ERR__PARTITION_EOF) {
                // End of partition, no action needed
            } 
            else {
                // Actual Error (e.g., SSL handshake failed or Network down)
                cerr << "❌ Consumer Error: " << rd_kafka_message_errstr(rkmessage) << endl;
            }
            rd_kafka_message_destroy(rkmessage);
        }
    }

    // 5. Cleanup
    cout << "\n🛑 Shutting down Order Service cleanly..." << endl;
    rd_kafka_consumer_close(rk);
    rd_kafka_destroy(rk);
    rd_kafka_topic_partition_list_destroy(topics);

    return 0;
}