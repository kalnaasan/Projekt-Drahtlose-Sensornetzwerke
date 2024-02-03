package edu.fra.uas.interiorsensors.common;

import edu.fra.uas.interiorsensors.model.Room;
import edu.fra.uas.interiorsensors.model.Sensor;
import edu.fra.uas.interiorsensors.model.ValueMeasure;
import edu.fra.uas.interiorsensors.repository.RoomRepository;
import edu.fra.uas.interiorsensors.repository.SensorRepository;
import edu.fra.uas.interiorsensors.repository.ValueMeasureRepository;
import jakarta.annotation.PostConstruct;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

import java.time.LocalDate;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Random;

@Slf4j
@Component
public class initDB {

    @Value("${spring.profiles.active}")
    private String activeProfile;
    private final RoomRepository roomRepository;
    private final SensorRepository sensorRepository;
    private final ValueMeasureRepository valueMeasureRepository;

    @Autowired
    public initDB(RoomRepository roomRepository, SensorRepository sensorRepository, ValueMeasureRepository valueMeasureRepository) {
        this.roomRepository = roomRepository;
        this.sensorRepository = sensorRepository;
        this.valueMeasureRepository = valueMeasureRepository;
    }

    @PostConstruct
    private void init() {
        if (this.activeProfile.equals("dev")) {
            Room room1 = this.roomRepository.save(new Room(null, "Room 1", new ArrayList<>(), null, null, LocalDateTime.now(), LocalDateTime.now()));
            this.genrateSensorsOfRoom(room1, "123456");
            Room room2 = this.roomRepository.save(new Room(null, "Room 2", new ArrayList<>(), null, null, LocalDateTime.now(), LocalDateTime.now()));
            this.genrateSensorsOfRoom(room2, "456789");
            Room room3 = this.roomRepository.save(new Room(null, "Room 3", new ArrayList<>(), null, null, LocalDateTime.now(), LocalDateTime.now()));
            this.genrateSensorsOfRoom(room3, "789123");
            Room room4 = this.roomRepository.save(new Room(null, "Room 4", new ArrayList<>(), null, null, LocalDateTime.now(), LocalDateTime.now()));
            this.genrateSensorsOfRoom(room4, "147258");

            this.sensorRepository.save(new Sensor(null, "369258_SD41_TEMP", "TEMP", null, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
            this.sensorRepository.save(new Sensor(null, "369258_SD41_CO2", "CO2", null, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
            this.sensorRepository.save(new Sensor(null, "369258_SD41_HUM", "HM", null, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
            this.sensorRepository.save(new Sensor(null, "369258_SD41_VOC", "VOC", null, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        }
    }

    private void genrateSensorsOfRoom(Room room, String board) {
        Sensor SVD41_TEMP = this.sensorRepository.save(new Sensor(null, board + "_SVD41_TEMP", "TEMP", room, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.generateValues(SVD41_TEMP, 12000, 29000, LocalDate.now());

        Sensor SVD41_CO2 = this.sensorRepository.save(new Sensor(null, board + "_SVD41_CO2", "CO2", room, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.generateValues(SVD41_CO2, 500, 2500, LocalDate.now());

        Sensor SVD41_HUM = this.sensorRepository.save(new Sensor(null, board + "_SVD41_HUM", "HUM", room, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.generateValues(SVD41_HUM, 30000, 80000, LocalDate.now());

        Sensor SVD41_VOC = this.sensorRepository.save(new Sensor(null, board + "_SVD41_VOC", "VOC", room, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.generateValues(SVD41_VOC, 200, 1200, LocalDate.now());
    }

    public void generateValues(Sensor sensor, Integer minValue, Integer maxValue, LocalDate date) {
        Random random = new Random();
        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 60; j += 3) {
                int randomInteger = random.nextInt((maxValue - minValue) + 1) + minValue;
                LocalDateTime created = LocalDateTime.of(date.getYear(), date.getMonthValue(), date.getDayOfMonth(), i % 24, j % 60, j % 60);
                ValueMeasure valueMeasure = new ValueMeasure(null, randomInteger, created, sensor, created, created);
                this.valueMeasureRepository.save(valueMeasure);
            }
        }
    }
}
