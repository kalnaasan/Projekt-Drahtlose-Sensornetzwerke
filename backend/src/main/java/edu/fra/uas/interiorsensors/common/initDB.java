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
            Room room1 = this.roomRepository.save(new Room(null, "Room 1", new ArrayList<>(),null, LocalDateTime.now(), LocalDateTime.now()));
            this.genrateSensorsOfRoom(room1, "123456");
            Room room2 = this.roomRepository.save(new Room(null, "Room 2", new ArrayList<>(),null, LocalDateTime.now(), LocalDateTime.now()));
            this.genrateSensorsOfRoom(room2, "456789");
            Room room3 = this.roomRepository.save(new Room(null, "Room 3", new ArrayList<>(),null, LocalDateTime.now(), LocalDateTime.now()));
            this.genrateSensorsOfRoom(room3, "789123");
           // Room room4 = this.roomRepository.save(new Room(null, "Room 4", new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
            this.genrateSensorsOfRoom(null, "147258");

            this.sensorRepository.save(new Sensor(null, "369258_SD41_TEMP", "TEMP", null, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
            this.sensorRepository.save(new Sensor(null, "369258_SD41_CO2", "CO2", null, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
            this.sensorRepository.save(new Sensor(null, "369258_SD41_HM", "HM", null, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
            this.sensorRepository.save(new Sensor(null, "369258_SD41_VOC", "VOC", null, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        }
    }

    private void genrateSensorsOfRoom(Room room, String board) {
        Sensor SD41_TEMP = this.sensorRepository.save(new Sensor(null, board + "_SD41_TEMP", "TEMP", room, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.generateValues(SD41_TEMP, 12, 29);

        Sensor SD41_CO2 = this.sensorRepository.save(new Sensor(null, board + "_SD41_CO2", "CO2", room, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.generateValues(SD41_CO2, 500, 2500);

        Sensor SD41_HM = this.sensorRepository.save(new Sensor(null, board + "_SD41_HM", "HM", room, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.generateValues(SD41_HM, 30, 80);

        Sensor SD41_VOC = this.sensorRepository.save(new Sensor(null, board + "_SD41_VOC", "VOC", room, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.generateValues(SD41_VOC, 20, 120);
    }

    public void generateValues(Sensor sensor, Integer minValue, Integer maxValue) {
        Random random = new Random();
        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 60; j+=3) {
                int randomInteger = random.nextInt((maxValue - minValue) + 1) + minValue;
                LocalDateTime created = LocalDateTime.of(LocalDate.now().getYear(), LocalDate.now().getMonthValue(), LocalDate.now().getDayOfMonth(), i % 24, j % 60, j % 60);
                ValueMeasure valueMeasure = new ValueMeasure(null, randomInteger, created, sensor, created, created);
                this.valueMeasureRepository.save(valueMeasure);
            }
        }
    }
}
