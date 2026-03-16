FROM gcc:latest

# Install all system dependencies including ASIO for Crow
RUN apt-get update && apt-get install -y \
    libmariadb-dev \
    libmariadb-dev-compat \
    librdkafka-dev \
    libhiredis-dev \
    libssl-dev \
    cmake \
    git \
    libasio-dev

WORKDIR /usr/src/app

# Copy your code and certificates
COPY . .

# 1. Install JWT-CPP (Identity)
RUN git clone https://github.com/Thalhammer/jwt-cpp.git /tmp/jwt-cpp-repo && \
    cp -r /tmp/jwt-cpp-repo/include/* /usr/local/include/ && \
    rm -rf /tmp/jwt-cpp-repo

# 2. Install Crow (Web Framework)
RUN git clone https://github.com/CrowCpp/Crow.git /tmp/crow-repo && \
    cp -r /tmp/crow-repo/include/* /usr/local/include/ && \
    rm -rf /tmp/crow-repo

# 3. Compile the service with Threading support (-lpthread)
RUN g++ -o user_service create_user.cpp -lmysqlclient -lrdkafka -lhiredis -lssl -lcrypto -lpthread

CMD ["./user_service"]