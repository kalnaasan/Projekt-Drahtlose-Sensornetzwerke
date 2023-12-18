package edu.fra.uas.interiorsensors.controller;

import edu.fra.uas.interiorsensors.Classes.ValueMeasure;
import edu.fra.uas.interiorsensors.common.ResponseMessage;
import edu.fra.uas.interiorsensors.repository.RoomRepository;
import edu.fra.uas.interiorsensors.repository.ValueMeasureRepository;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.List;
import java.util.Optional;
import java.util.UUID;

@RestController
@RequestMapping("/api/v1/web/values")
public class ValueMeasureController {
    private final ValueMeasureRepository valueMeasureRepository;

    private final Logger logger = LoggerFactory.getLogger(RoomRepository.class);
    @Autowired
    public ValueMeasureController(ValueMeasureRepository valueMeasureRepository) {
        this.valueMeasureRepository = valueMeasureRepository;
    }



    @GetMapping(produces = MediaType.APPLICATION_JSON_VALUE)
    public ResponseEntity<ResponseMessage> index(@RequestParam(defaultValue = "0") int page, @RequestParam(defaultValue = "10") int size){
        logger.debug("Indexing ValueMeasure : {}",this.valueMeasureRepository);
        List<ValueMeasure> values = valueMeasureRepository.findAll();
        return this.message("Indexing Values",values, HttpStatus.OK);
    }

    @GetMapping("/{id}")
    public ResponseEntity<ResponseMessage> getById(@PathVariable UUID id) {
        logger.debug("Getting ValueMeasure by id: {} ", id);
        Optional<ValueMeasure> optionalvalue = this.valueMeasureRepository.findById(id);
        return optionalvalue.map(
                        value -> this.message("Getting Room by id", value, HttpStatus.OK))
                .orElseGet(() -> this.message("Room not found", null, HttpStatus.NOT_FOUND));
    }

    @PostMapping
    public ResponseEntity<ResponseMessage> create(@RequestBody ValueMeasure valueMeasure) {
        logger.debug("Create ValueMeasure: {}", valueMeasure);

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
        logger.debug("Updating User by id: {}", id);
        Optional<ValueMeasure> optionalValue = this.valueMeasureRepository.findById(id);
        if (optionalValue.isPresent() && optionalValue.get().getId().equals(valueMeasure.getId())) {

            ValueMeasure valueMeasureCreated = this.valueMeasureRepository.save(valueMeasure);

            return this.message("Updating valueMeasure by id", valueMeasureCreated, HttpStatus.ACCEPTED);
        }
        return this.message("valueMeasure not found", null, HttpStatus.NOT_FOUND);
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<ResponseMessage> delete(@PathVariable("id") UUID id){
        logger.debug("Deleting ValueMeasure by id: {}", id);
        Optional<ValueMeasure> ValueMeasureUpdate = this.valueMeasureRepository.findById(id);

        if (ValueMeasureUpdate.isPresent()){
            //this.valueMeasureRepository.deleteById(id);
            return this.message("ValueMeasure is deleted", null , HttpStatus.NO_CONTENT);
        }
        return this.message("ValueMeasure is not found", null , HttpStatus.NO_CONTENT);
    }



    private ResponseEntity<ResponseMessage> message(String message, Object data, HttpStatus httpStatus) {
        return new ResponseEntity<>(new ResponseMessage(message, data), httpStatus);
    }

}
