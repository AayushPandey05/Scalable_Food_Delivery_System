# Use a stable GCC base image
FROM gcc:latest

# Install system dependencies and librdkafka
RUN apt-get update && apt-get install -y \
    librdkafka-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
WORKDIR /usr/src/app

# Create the ssl directory structure to match your C++ code paths
RUN mkdir -p /usr/src/app/ssl

# Copy all files from order-service folder to the container
COPY . .

# Compile the C++ service with optimization flags and link rdkafka
# -O3 makes the code faster, -Wall shows all warnings for better debugging
RUN g++ -O3 -Wall -o order_service order_service.cpp -lrdkafka

# Environment variable to help with debugging (optional)
ENV KAFKA_LOG_LEVEL=7

# Final command to run the binary
CMD ["./order_service"]