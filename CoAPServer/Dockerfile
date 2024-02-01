# Use the Debian:bookworm base image
FROM debian:bookworm

ENV TZ=Europe/Berlin
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Update and upgrade the system
RUN apt update && apt -y upgrade

# Install OpenJDK 17
RUN apt -y install openjdk-17-jdk

# Add the JAR file to the container
ADD target/Server-1.0.0.jar Server-1.0.0.jar

# Expose the container to listen on port 8080
EXPOSE 5683

# Set the entry point command for the Java application
ENTRYPOINT ["java", "-jar", "Server-1.0.0.jar", "-Djava.net.preferIPv4Stack=true"]
