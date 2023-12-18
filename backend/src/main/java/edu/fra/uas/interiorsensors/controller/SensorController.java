package edu.fra.uas.interiorsensors.controller;


import edu.fra.uas.interiorsensors.Classes.DTOValueMeasure;
import edu.fra.uas.interiorsensors.Classes.Sensor;
import edu.fra.uas.interiorsensors.Classes.ValueMeasure;
import edu.fra.uas.interiorsensors.common.ResponseMessage;
import edu.fra.uas.interiorsensors.repository.SensorRepository;
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
@RequestMapping("/api/v1/web/sensors")
public class SensorController {

    private final SensorRepository sensorRepository;
    private final ValueMeasureRepository valueMeasureRepository;

    private final Logger logger = LoggerFactory.getLogger(SensorRepository.class);

    @Autowired

    public SensorController(SensorRepository sensorRepository, ValueMeasureRepository valueMeasureRepository) {
        this.sensorRepository = sensorRepository;
        this.valueMeasureRepository = valueMeasureRepository;
    }


    @GetMapping(produces = MediaType.APPLICATION_JSON_VALUE)
    public ResponseEntity<ResponseMessage> index(@RequestParam(defaultValue = "0") int page, @RequestParam(defaultValue = "10") int size){
        logger.debug("Indexing Sensor : {}",this.sensorRepository);
        List<Sensor> sensors = sensorRepository.findAll();
        return this.message("Indexing Sensor",sensors, HttpStatus.OK);
    }

    @GetMapping("/{id}")
    public ResponseEntity<ResponseMessage> getById(@PathVariable UUID id) {
        logger.debug("Getting Sensor by id: {} ", id);
        Optional<Sensor> optionalSensor = this.sensorRepository.findById(id);
        return optionalSensor.map(
                        sensor -> this.message("Getting Sensor by id", sensor, HttpStatus.OK))
                .orElseGet(() -> this.message("Sensor not found", null, HttpStatus.NOT_FOUND));
    }

    @PostMapping
    public ResponseEntity<ResponseMessage> create(@RequestBody Sensor sensor){
        logger.debug("create sensor: {}", sensor);
        Optional<Sensor> optionalSensor = (sensor.getId()!= null)? this.sensorRepository.findById(sensor.getId()) : Optional.empty();

        if (optionalSensor.isPresent() && sensorRepository.existsByName(sensor.getName())){
            return this.message("Sensor is already exists", null, HttpStatus.CONFLICT);
        }
        Sensor sensorCreated = this.sensorRepository.save(sensor);
        return this.message("Created Room",sensorCreated,HttpStatus.CREATED);
    }

    @PutMapping("/{id}")
    public ResponseEntity<ResponseMessage> update(@PathVariable("id") UUID id, @RequestBody Sensor sensor) {
        logger.debug("Updating Sensor by id: {}", id);
        Optional<Sensor> optionalSensor = this.sensorRepository.findById(id);
        if (optionalSensor.isPresent() && optionalSensor.get().getId().equals(sensor.getId())) {

            Sensor sensorCreated = this.sensorRepository.save(sensor);

            return this.message("Updating Sensor by id", sensorCreated, HttpStatus.ACCEPTED);
        }
        return this.message("Sensor not found", null, HttpStatus.NOT_FOUND);
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<ResponseMessage> delete(@PathVariable("id") UUID id){
        logger.debug("Deleting Sensor by id: {}", id);
        Optional<Sensor> sensorUpdate = this.sensorRepository.findById(id);

        if (sensorUpdate.isPresent()){
            this.sensorRepository.deleteById(id);
            return this.message("Sensor is deleted", null , HttpStatus.NO_CONTENT);
        }
        return this.message("Sensor is not found", null , HttpStatus.NO_CONTENT);
    }

    @RequestMapping("/add/values")
    public ResponseEntity<ResponseMessage> addValues(@RequestBody DTOValueMeasure valueMeasure){
        logger.debug("Adding values to Sensor");
        for(String key : valueMeasure.getValues().keySet()) {
            String name = valueMeasure.getId() + '_' + key;

            Optional<Sensor> sensorOptional = this.sensorRepository.findByName(name);
            Sensor sensor;
            if (sensorOptional.isEmpty()){
                // create sensor
                 sensor = new Sensor();
                sensor.setName(name);
                sensor.setType(key.split("_")[1]);
               sensor = this.sensorRepository.save(sensor);
            } else{
                sensor= sensorOptional.get();
            }
            // create value for the sensor
            ValueMeasure value = new ValueMeasure();
            value.setValue(valueMeasure.getValues().get(key));
            value.setSensor(sensor);
            this.valueMeasureRepository.save(value);

        }
        return this.message("Values are stored",null,HttpStatus.CREATED);

    }

    private ResponseEntity<ResponseMessage> message(String message, Object data, HttpStatus httpStatus) {
        return new ResponseEntity<>(new ResponseMessage(message, data), httpStatus);
    }

}
