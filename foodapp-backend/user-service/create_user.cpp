#include "RedisManager.h"        
#include <mysql/mysql.h>        
#include <librdkafka/rdkafka.h> 
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>

using namespace std;

// Kafka Event Function (Purana Logic)
void send_user_event(const char* username) {
    char errstr[512];
    rd_kafka_conf_t *conf = rd_kafka_conf_new();
    rd_kafka_conf_set(conf, "bootstrap.servers", "foodapp-kafka-broker-lpu-e79e.k.aivencloud.com:21836", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "security.protocol", "ssl", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "ssl.ca.location", "certificates/ca.pem", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "ssl.certificate.location", "certificates/service.cert", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "ssl.key.location", "certificates/service.key", errstr, sizeof(errstr));

    rd_kafka_t *rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
    if (!rk) { return; }

    string message = "{\"event\": \"USER_CREATED\", \"username\": \"" + string(username) + "\"}";
    rd_kafka_produce(rd_kafka_topic_new(rk, "order-events", NULL), RD_KAFKA_PARTITION_UA, RD_KAFKA_MSG_F_COPY, (void*)message.c_str(), message.length(), NULL, 0, NULL);

    rd_kafka_flush(rk, 5000); 
    rd_kafka_destroy(rk);
}

int main() {
    MYSQL* conn = mysql_init(NULL);
    
    // RDS Config
    string host = "foodapp-db.clyk8ag4gkbt.ap-south-1.rds.amazonaws.com";
    string user = "admin";
    string pass = "76072pandey";
    string db   = "foodapp";

    if (mysql_real_connect(conn, host.c_str(), user.c_str(), pass.c_str(), db.c_str(), 3306, NULL, 0)) {
        cout << "✅ Connected to RDS!" << endl;

        // --- NEW USER DATA ---
        string username = "Pratyush_Modular_01"; 
        string email = "modular@example.com";
        string pass_hash = "hashed_pw_123"; 

        string query = "INSERT INTO users (username, email, password_hash) VALUES ('" + 
                        username + "', '" + email + "', '" + pass_hash + "')";

        if (mysql_query(conn, query.c_str())) {
            cout << "❌ Insert Error: " << mysql_error(conn) << endl;
        } 
        else {
            cout << "🚀 Success! User added to RDS." << endl;

            // 🔥 1. REDIS CACHE (Modular call)
            RedisManager::cacheUser(username, email);

            // 🔥 2. KAFKA EVENT
            send_user_event(username.c_str());
        }
    } else {
        cout << "❌ RDS Connection Error: " << mysql_error(conn) << endl;
    }

    mysql_close(conn);
    return 0;
}