package edu.fra.uas.interiorsensors.common;

import edu.fra.uas.interiorsensors.model.Room;
import edu.fra.uas.interiorsensors.model.Sensor;
import edu.fra.uas.interiorsensors.repository.RoomRepository;
import edu.fra.uas.interiorsensors.repository.SensorRepository;
import jakarta.annotation.PostConstruct;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.time.LocalDateTime;
import java.util.ArrayList;

@Component
public class initDB {

    private final RoomRepository roomRepository;
    private final SensorRepository sensorRepository;

    @Autowired
    public initDB(RoomRepository roomRepository, SensorRepository sensorRepository) {
        this.roomRepository = roomRepository;
        this.sensorRepository = sensorRepository;
    }

    @PostConstruct
    private void init() {
        Room room1 = this.roomRepository.save(new Room(null, "Room 1", new ArrayList<>(), LocalDateTime.now(),LocalDateTime.now()));
        Room room2 = this.roomRepository.save(new Room(null, "Room 2", new ArrayList<>(), LocalDateTime.now(),LocalDateTime.now()));
        Room room3 = this.roomRepository.save(new Room(null, "Room 3", new ArrayList<>(), LocalDateTime.now(),LocalDateTime.now()));
        Room room4 = this.roomRepository.save(new Room(null, "Room 4", new ArrayList<>(), LocalDateTime.now(),LocalDateTime.now()));

        this.sensorRepository.save(new Sensor(null, "123456_SD41_TEMP", "TEMP", room1, new ArrayList<>(), LocalDateTime.now(),LocalDateTime.now()));
        this.sensorRepository.save(new Sensor(null, "123456_SD41_CO2", "CO2", room1, new ArrayList<>(), LocalDateTime.now(),LocalDateTime.now()));
        this.sensorRepository.save(new Sensor(null, "123456_SD41_HM", "HM", room1, new ArrayList<>(), LocalDateTime.now(),LocalDateTime.now()));
        this.sensorRepository.save(new Sensor(null, "123456_SD41_VOC", "VOC", room1, new ArrayList<>(), LocalDateTime.now(),LocalDateTime.now()));

        this.sensorRepository.save(new Sensor(null, "587456_SD41_TEMP", "TEMP", null, new ArrayList<>(), LocalDateTime.now(),LocalDateTime.now()));
        this.sensorRepository.save(new Sensor(null, "587456_SD41_CO2", "CO2", null, new ArrayList<>(), LocalDateTime.now(),LocalDateTime.now()));
        this.sensorRepository.save(new Sensor(null, "587456_SD41_HM", "HM", null, new ArrayList<>(), LocalDateTime.now(),LocalDateTime.now()));
        this.sensorRepository.save(new Sensor(null, "587456_SD41_VOC", "VOC", null, new ArrayList<>(), LocalDateTime.now(),LocalDateTime.now()));

        this.sensorRepository.save(new Sensor(null, "876432_SD41_TEMP", "TEMP", null, new ArrayList<>(), LocalDateTime.now(),LocalDateTime.now()));
        this.sensorRepository.save(new Sensor(null, "876432_SD41_CO2", "CO2", null, new ArrayList<>(), LocalDateTime.now(),LocalDateTime.now()));
        this.sensorRepository.save(new Sensor(null, "876432_SD41_HM", "HM", null, new ArrayList<>(), LocalDateTime.now(),LocalDateTime.now()));
        this.sensorRepository.save(new Sensor(null, "876432_SD41_VOC", "VOC", null, new ArrayList<>(), LocalDateTime.now(),LocalDateTime.now()));
    }
}
