#include "crow.h"
#include "RedisManager.h"        
#include <mysql/mysql.h>         
#include <librdkafka/rdkafka.h> 
#include <jwt-cpp/jwt.h> 
#include <iostream>
#include <string>
#include <chrono>

using namespace std;

// Generate JWT for Aayush's App
string generate_auth_token(string username, string email) {
    auto token = jwt::create()
        .set_type("JWS")
        .set_issuer("foodapp-auth")
        .set_issued_at(chrono::system_clock::now())
        .set_expires_at(chrono::system_clock::now() + chrono::hours{24})
        .set_payload_claim("username", jwt::claim(username))
        .set_payload_claim("email", jwt::claim(email))
        .set_payload_claim("scope", jwt::claim(string("user:standard"))) 
        // 🔥 Updated secret key for Aayush
        .sign(jwt::algorithm::hs256{"aayush_secret_key_76072"}); 
    return token;
}

// Kafka logic (remains same)
void send_identity_and_event(const char* username, string jwt_token) {
    char errstr[512];
    rd_kafka_conf_t *conf = rd_kafka_conf_new();
    rd_kafka_conf_set(conf, "bootstrap.servers", "foodapp-kafka-broker-lpu-e79e.k.aivencloud.com:21836", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "security.protocol", "ssl", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "ssl.ca.location", "certificates/ca.pem", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "ssl.certificate.location", "certificates/service.cert", errstr, sizeof(errstr));
    rd_kafka_conf_set(conf, "ssl.key.location", "certificates/service.key", errstr, sizeof(errstr));

    rd_kafka_t *rk = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
    if (!rk) return;

    string business_msg = "{\"event\": \"USER_CREATED\", \"username\": \"" + string(username) + "\"}";
    rd_kafka_topic_t *rkt_biz = rd_kafka_topic_new(rk, "user-events", NULL);
    rd_kafka_produce(rkt_biz, RD_KAFKA_PARTITION_UA, RD_KAFKA_MSG_F_COPY, (void*)business_msg.c_str(), business_msg.length(), NULL, 0, NULL);

    string audit_msg = "{\"action\": \"IDENTITY_ISSUED\", \"user\": \"" + string(username) + "\", \"token\": \"" + jwt_token + "\"}";
    rd_kafka_topic_t *rkt_audit = rd_kafka_topic_new(rk, "identity-audit-logs", NULL);
    rd_kafka_produce(rkt_audit, RD_KAFKA_PARTITION_UA, RD_KAFKA_MSG_F_COPY, (void*)audit_msg.c_str(), audit_msg.length(), NULL, 0, NULL);

    rd_kafka_flush(rk, 5000); 
    rd_kafka_topic_destroy(rkt_biz);
    rd_kafka_topic_destroy(rkt_audit);
    rd_kafka_destroy(rk);
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/register").methods("POST"_method, "OPTIONS"_method)([](const crow::request& req){
        crow::response res;
        
        // ✅ CORS Headers
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");

        if (req.method == "OPTIONS"_method) {
            res.code = 204;
            return res;
        }

        auto x = crow::json::load(req.body);
        if (!x) {
            res.code = 400;
            res.body = "Invalid JSON";
            return res;
        }

        string username = x["username"].s();
        string email = x["email"].s();

        MYSQL* conn = mysql_init(NULL);
        if (mysql_real_connect(conn, "foodapp-db.clyk8ag4gkbt.ap-south-1.rds.amazonaws.com", "admin", "76072pandey", "foodapp", 3306, NULL, 0)) {
            
            string query = "INSERT INTO users (username, email, password_hash) VALUES ('" + username + "', '" + email + "', 'secure_hash')";
            
            if (!mysql_query(conn, query.c_str())) {
                RedisManager::cacheUser(username, email);
                string jwt_token = generate_auth_token(username, email);
                send_identity_and_event(username.c_str(), jwt_token);

                mysql_close(conn);
                
                crow::json::wvalue success_res;
                success_res["status"] = "success";
                success_res["token"] = jwt_token;
                
                res.code = 200;
                res.body = success_res.dump();
                return res;
            }
        }
        
        if (conn) mysql_close(conn);
        res.code = 500;
        res.body = "Database Error or Duplicate User";
        return res;
    });

    // 🔥 Log updated for Aayush
    cout << "🚀 Aayush's FoodApp Identity Service listening on Port 8080..." << endl;
    app.port(8080).multithreaded().run();
}