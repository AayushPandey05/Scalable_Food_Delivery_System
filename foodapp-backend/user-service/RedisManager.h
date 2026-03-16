//!.....REDIS MANAGER.......
#ifndef REDIS_MANAGER_H
#define REDIS_MANAGER_H

#include <hiredis/hiredis.h>
#include <iostream>
#include <string>

class RedisManager {
public:
    // Added std:: before string, cerr, and cout
    static void cacheUser(std::string username, std::string email) {
        redisContext *c = redisConnect("127.0.0.1", 6379);

        if (c == NULL || c->err) {
            std::cerr << "⚠️ Redis Error: Connection failed" << std::endl;
            if (c) redisFree(c);
            return;
        }

        std::string key = "user:" + username;
        std::string val = "{\"email\":\"" + email + "\", \"status\":\"active\"}";

        // SETEX: key, expiry_seconds, value
        redisReply *reply = (redisReply *)redisCommand(c, "SETEX %s 3600 %s", key.c_str(), val.c_str());

        if (reply != NULL) {
            std::cout << "⚡ [Cache Service] User " << username << " stored in Redis." << std::endl;
            freeReplyObject(reply);
        }

        redisFree(c);
    }
};

#endif