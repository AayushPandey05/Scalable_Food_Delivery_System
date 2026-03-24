#ifndef REDIS_CLIENT_H
#define REDIS_CLIENT_H

#include <sw/redis++/redis++.h>
#include <iostream>
#include <string>
#include <vector>

using namespace sw::redis;

class RedisClient {
private:
    Redis* redis;

public:
    // Constructor to connect to Redis Cloud
    RedisClient(const std::string& host, int port, const std::string& password) {
        try {
            ConnectionOptions opts;
            opts.host = host;
            opts.port = port;
            opts.password = password;
            opts.socket_timeout = std::chrono::milliseconds(200);

            redis = new Redis(opts);
            std::cout << "✅ Connected to Redis Cloud Successfully!" << std::endl;
        } catch (const Error &e) {
            std::cerr << "❌ Redis Connection Error: " << e.what() << std::endl;
        }
    }

    // Save Cart Item (Key: "cart:user_id", Value: "item_details")
    void saveCart(const std::string& userId, const std::string& cartData) {
        redis->set("cart:" + userId, cartData);
        std::cout << "🛒 Redis: Cart updated for User " << userId << std::endl;
    }

    // Retrieve Cart for Processing
    std::string getCart(const std::string& userId) {
        auto val = redis->get("cart:" + userId);
        if (val) return *val;
        return "EMPTY";
    }

    // Clear Cart after Order is Placed (As seen in your Project requirements)
    void clearCart(const std::string& userId) {
        redis->del("cart:" + userId);
        std::cout << "🧹 Redis: Cart cleared for User " << userId << " after order." << std::endl;
    }

    ~RedisClient() {
        delete redis;
    }
};

#endif