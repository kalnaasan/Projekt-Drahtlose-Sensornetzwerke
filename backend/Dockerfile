# Use the Debian:bookworm base image
FROM debian:bookworm

# Update and upgrade the system
RUN apt update && apt -y upgrade

# Install OpenJDK 17
RUN apt -y install openjdk-17-jdk

# Install FFmpeg
RUN apt -y install ffmpeg

# Check the FFmpeg version
RUN ffmpeg -version

# Add the JAR file to the container
ADD target/InteriorSensors-1.0.0.jar InteriorSensors-1.0.0.jar

# Expose the container to listen on port 8080
EXPOSE 8080

# Set the entry point command for the Java application
ENTRYPOINT ["java", "-jar", "InteriorSensors-1.0.0.jar"]
