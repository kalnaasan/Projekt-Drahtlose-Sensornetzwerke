package edu.fra.uas.interiorsensors.controller;

import edu.fra.uas.interiorsensors.common.ResponseMessage;
import edu.fra.uas.interiorsensors.model.ValueMeasure;
import edu.fra.uas.interiorsensors.repository.ValueMeasureRepository;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.time.Duration;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.UUID;

@Slf4j
@CrossOrigin(origins = "*")
@RestController
@RequestMapping("values")
public class ValueMeasureController {
    private final ValueMeasureRepository valueMeasureRepository;

    @Autowired
    public ValueMeasureController(ValueMeasureRepository valueMeasureRepository) {
        this.valueMeasureRepository = valueMeasureRepository;
    }

    @GetMapping(produces = MediaType.APPLICATION_JSON_VALUE)
    public ResponseEntity<ResponseMessage> index(@RequestParam(value = "sensor-name") String sensorName,
                                                 @RequestParam(value = "from") String from,
                                                 @RequestParam(value = "to") String to) {
        log.debug("Indexing ValueMeasure : {}", this.valueMeasureRepository.count());
        LocalDateTime fromDate = LocalDateTime.parse(from);
        LocalDateTime toDate = LocalDateTime.parse(to);
        if ((toDate.getHour() - fromDate.getHour() > 12)) {
            List<Object[]> values = valueMeasureRepository.findAllBySensor_NameAndReadAtBetweenAndAverageReadAtOrderByReadAt(sensorName, fromDate, toDate);
            List<ValueMeasure> valueMeasures = new ArrayList<>();
            for (Object[] value : values) {
                valueMeasures.add(new ValueMeasure(null, (Double) value[1], fromDate.plusHours((Integer) value[0]), null, null, null));
            }
            return this.message("Indexing Values", valueMeasures, HttpStatus.OK);
        } else if (Duration.between(fromDate,toDate).toDays() == 7) {
            return this.message("Indexing Values", this.getValueMeasureByWeek(sensorName, fromDate), HttpStatus.OK);
        }
        List<ValueMeasure> values = valueMeasureRepository.findAllBySensor_NameAndReadAtBetweenOrderByReadAtAsc(sensorName, fromDate, toDate);
        return this.message("Indexing Values", values, HttpStatus.OK);
    }

    @GetMapping("/{id}")
    public ResponseEntity<ResponseMessage> getById(@PathVariable UUID id) {
        log.debug("Getting ValueMeasure by id: {} ", id);
        Optional<ValueMeasure> optionalValue = this.valueMeasureRepository.findById(id);
        return optionalValue.map(
                        value -> this.message("Getting Room by id", value, HttpStatus.OK))
                .orElseGet(() -> this.message("Room not found", null, HttpStatus.NOT_FOUND));
    }

    @PostMapping
    public ResponseEntity<ResponseMessage> create(@RequestBody ValueMeasure valueMeasure) {
        log.debug("Create ValueMeasure: {}", valueMeasure);

        // Überprüfe, ob ein ValueMeasure mit der gegebenen ID bereits existiert
        if (valueMeasure.getId() != null && valueMeasureRepository.existsById(valueMeasure.getId())) {
            return this.message("ValueMeasure with the given ID already exists", null, HttpStatus.CONFLICT);
        }

        // Überprüfe, ob ein ValueMeasure mit dem gegebenen Wert bereits existiert
        if (valueMeasureRepository.existsByValue(valueMeasure.getValue())) {
            return this.message("ValueMeasure with the given value already exists", null, HttpStatus.CONFLICT);
        }

        // Speichere das ValueMeasure in der Datenbank
        ValueMeasure valueMeasureCreated = this.valueMeasureRepository.save(valueMeasure);

        // Gebe eine Erfolgsmeldung zurück
        return this.message("Created ValueMeasure", valueMeasureCreated, HttpStatus.CREATED);
    }

    @PutMapping("/{id}")
    public ResponseEntity<ResponseMessage> update(@PathVariable("id") UUID id, @RequestBody ValueMeasure valueMeasure) {
        log.debug("Updating User by id: {}", id);
        Optional<ValueMeasure> optionalValue = this.valueMeasureRepository.findById(id);
        if (optionalValue.isPresent() && optionalValue.get().getId().equals(valueMeasure.getId())) {

            ValueMeasure valueMeasureCreated = this.valueMeasureRepository.save(valueMeasure);

            return this.message("Updating valueMeasure by id", valueMeasureCreated, HttpStatus.ACCEPTED);
        }
        return this.message("valueMeasure not found", null, HttpStatus.NOT_FOUND);
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<ResponseMessage> delete(@PathVariable("id") UUID id) {
        log.debug("Deleting ValueMeasure by id: {}", id);
        Optional<ValueMeasure> ValueMeasureUpdate = this.valueMeasureRepository.findById(id);

        if (ValueMeasureUpdate.isPresent()) {
            this.valueMeasureRepository.deleteById(id);
            return this.message("ValueMeasure is deleted", null, HttpStatus.NO_CONTENT);
        }
        return this.message("ValueMeasure is not found", null, HttpStatus.NO_CONTENT);
    }

    private ResponseEntity<ResponseMessage> message(String message, Object data, HttpStatus httpStatus) {
        return new ResponseEntity<>(new ResponseMessage(message, data), httpStatus);
    }

    private List<ValueMeasure> getValueMeasureByWeek(String sensorName, LocalDateTime start){
        List<ValueMeasure> valueMeasures = new ArrayList<>();
        for (int i = 0; i <8; i++){
            LocalDateTime from = start.plusDays(i);
            LocalDateTime to = start.plusDays((i+1));
            List<Object[]> values = valueMeasureRepository.findAllBySensor_NameAndReadAtBetweenAndAverageByFourHours_ReadAtOrderByReadAt(sensorName, from, to);
            for (Object[] value : values) {
                valueMeasures.add(new ValueMeasure(null, (Double) value[1], from.plusHours((Integer) value[0]), null, null, null));
            }
        }
        return valueMeasures;
    }
}
