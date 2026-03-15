FROM gcc:latest

# Install dependencies
RUN apt-get update && apt-get install -y \
    libmariadb-dev \
    libmariadb-dev-compat \
    librdkafka-dev \
    libhiredis-dev

WORKDIR /usr/src/app

# Sab files (including RedisManager.h) copy karo
COPY . .

# Yeh compilation Docker ke andar hoga
RUN g++ -o user_service create_user.cpp -lmysqlclient -lrdkafka -lhiredis

CMD ["./user_service"]