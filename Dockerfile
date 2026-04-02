FROM debian:bookworm-slim

# Prevent apt from stopping to ask for user input (like timezone prompts)
ENV DEBIAN_FRONTEND=noninteractive

# Install all the system dependencies required by your project
RUN apt-get update && apt-get install -y \
    gcc \
    make \
    python3 \
    python3-venv \
    python3-pip \
    nodejs \
    npm \
    iverilog \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
WORKDIR /app

# Copy the rest of the project files into the container
COPY . /app

# Ensure the bash script has execution permissions
RUN chmod +x run.sh

# By default, open an interactive bash shell when the container starts
CMD ["/bin/bash"]
