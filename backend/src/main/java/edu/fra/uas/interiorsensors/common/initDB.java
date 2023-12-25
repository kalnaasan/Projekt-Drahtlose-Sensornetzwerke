package edu.fra.uas.interiorsensors.common;

import edu.fra.uas.interiorsensors.model.Room;
import edu.fra.uas.interiorsensors.model.Sensor;
import edu.fra.uas.interiorsensors.model.ValueMeasure;
import edu.fra.uas.interiorsensors.repository.RoomRepository;
import edu.fra.uas.interiorsensors.repository.SensorRepository;
import edu.fra.uas.interiorsensors.repository.ValueMeasureRepository;
import jakarta.annotation.PostConstruct;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Random;

@Component
public class initDB {

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
        Room room1 = this.roomRepository.save(new Room(null, "Room 1", new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        Room room2 = this.roomRepository.save(new Room(null, "Room 2", new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        Room room3 = this.roomRepository.save(new Room(null, "Room 3", new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        Room room4 = this.roomRepository.save(new Room(null, "Room 4", new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));

        Sensor SD41_TEMP_123456 = this.sensorRepository.save(new Sensor(null, "123456_SD41_TEMP", "TEMP", room1, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.genrateTemperatureValues(SD41_TEMP_123456);
        Sensor SD41_CO2_123456 = this.sensorRepository.save(new Sensor(null, "123456_SD41_CO2", "CO2", room1, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.genrateCO2Values(SD41_CO2_123456);
        Sensor SD41_HM_123456 = this.sensorRepository.save(new Sensor(null, "123456_SD41_HM", "HM", room1, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.genrateHumidityValues(SD41_HM_123456);
        Sensor SD41_VOC_123456 = this.sensorRepository.save(new Sensor(null, "123456_SD41_VOC", "VOC", room1, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.genrateVOCValues(SD41_VOC_123456);

        Sensor SD41_TEMP_587456 = this.sensorRepository.save(new Sensor(null, "587456_SD41_TEMP", "TEMP", null, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.genrateTemperatureValues(SD41_TEMP_587456);
        Sensor SD41_CO2_587456 = this.sensorRepository.save(new Sensor(null, "587456_SD41_CO2", "CO2", null, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.genrateCO2Values(SD41_CO2_587456);
        Sensor SD41_HM_587456 = this.sensorRepository.save(new Sensor(null, "587456_SD41_HM", "HM", null, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.genrateHumidityValues(SD41_HM_587456);
        Sensor SD41_VOC_587456 = this.sensorRepository.save(new Sensor(null, "587456_SD41_VOC", "VOC", null, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.genrateVOCValues(SD41_VOC_587456);

        Sensor SD41_TEMP_876432 = this.sensorRepository.save(new Sensor(null, "876432_SD41_TEMP", "TEMP", null, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.genrateTemperatureValues(SD41_TEMP_876432);
        Sensor SD41_CO2_876432 = this.sensorRepository.save(new Sensor(null, "876432_SD41_CO2", "CO2", null, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.genrateCO2Values(SD41_CO2_876432);
        Sensor SD41_HM_876432 = this.sensorRepository.save(new Sensor(null, "876432_SD41_HM", "HM", null, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.genrateHumidityValues(SD41_HM_876432);
        Sensor SD41_VOC_876432 = this.sensorRepository.save(new Sensor(null, "876432_SD41_VOC", "VOC", null, new ArrayList<>(), LocalDateTime.now(), LocalDateTime.now()));
        this.genrateVOCValues(SD41_VOC_876432);

    }

    private void genrateTemperatureValues(Sensor sensor) {
        // Erstelle eine Instanz der Random-Klasse
        Random random = new Random();
        // Definiere den gewünschten Bereich
        double minValue = 10.0;
        double maxValue = 25.0;
        for (int i = 0; i < 720; i++) {
            // Generiere eine Zufallszahl im Bereich [minValue, maxValue)
            double randomDouble = Math.round((minValue + (maxValue - minValue) * random.nextDouble()) * 100.0) / 100.0;
            LocalDateTime created = LocalDateTime.of(2023, 12, 2 + i / 24, i % 24, 0);
            ValueMeasure valueMeasure = new ValueMeasure(null, randomDouble, created, sensor, created, created);
            this.valueMeasureRepository.save(valueMeasure);
        }
    }

    private void genrateCO2Values(Sensor sensor) {
        // Erstelle eine Instanz der Random-Klasse
        Random random = new Random();
        // Definiere den gewünschten Bereich
        int minValue = 500;
        int maxValue = 2500;
        for (int i = 0; i < 720; i++) {
            // Generiere eine Zufallszahl im Bereich [minValue, maxValue)
            int randomInteger = random.nextInt((maxValue - minValue) + 1) + minValue;
            LocalDateTime created = LocalDateTime.of(2023, 12, 2 + i / 24, i % 24, 0);
            ValueMeasure valueMeasure = new ValueMeasure(null, randomInteger, created, sensor, created, created);
            this.valueMeasureRepository.save(valueMeasure);
        }
    }

    private void genrateHumidityValues(Sensor sensor) {
        // Erstelle eine Instanz der Random-Klasse
        Random random = new Random();
        // Definiere den gewünschten Bereich
        int minValue = 30;
        int maxValue = 80;
        for (int i = 0; i < 720; i++) {
            // Generiere eine Zufallszahl im Bereich [minValue, maxValue)
            int randomInteger = random.nextInt((maxValue - minValue) + 1) + minValue;
            LocalDateTime created = LocalDateTime.of(2023, 12, 2 + i / 24, i % 24, 0);
            ValueMeasure valueMeasure = new ValueMeasure(null, randomInteger, created, sensor, created, created);
            this.valueMeasureRepository.save(valueMeasure);
        }
    }
    private void genrateVOCValues(Sensor sensor) {
        // Erstelle eine Instanz der Random-Klasse
        Random random = new Random();
        // Definiere den gewünschten Bereich
        int minValue = 20;
        int maxValue = 120;
        for (int i = 0; i < 720; i++) {
            // Generiere eine Zufallszahl im Bereich [minValue, maxValue)
            int randomInteger = random.nextInt((maxValue - minValue) + 1) + minValue;
            LocalDateTime created = LocalDateTime.of(2023, 12, 2 + i / 24, i % 24, 0);
            ValueMeasure valueMeasure = new ValueMeasure(null, randomInteger, created, sensor, created, created);
            this.valueMeasureRepository.save(valueMeasure);
        }
    }
}
