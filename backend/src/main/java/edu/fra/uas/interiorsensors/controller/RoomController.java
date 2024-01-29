package edu.fra.uas.interiorsensors.controller;


import edu.fra.uas.interiorsensors.common.ResponseMessage;
import edu.fra.uas.interiorsensors.model.*;
import edu.fra.uas.interiorsensors.repository.RoomRepository;

import edu.fra.uas.interiorsensors.repository.SensorRepository;

import edu.fra.uas.interiorsensors.repository.ValueMeasureRepository;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.domain.Limit;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.UUID;
import java.util.stream.Collectors;

@Slf4j
@CrossOrigin(origins = "*")
@RestController
@RequestMapping("rooms")
public class RoomController {

    private final RoomRepository roomRepository;
    private final SensorRepository sensorRepository;

    private final ValueMeasureRepository valueMeasureRepository;

    @Autowired
    public RoomController(RoomRepository roomRepository, SensorRepository sensorRepository, ValueMeasureRepository valueMeasureRepository) {
        this.roomRepository = roomRepository;
        this.sensorRepository = sensorRepository;
        this.valueMeasureRepository = valueMeasureRepository;
    }

    @GetMapping
    public ResponseEntity<ResponseMessage> index(@RequestParam (value = "shape",defaultValue = "false") boolean shape) {
        log.debug("Indexing Room : {}", this.roomRepository);
        if (shape == false) {
            List<Room> rooms = roomRepository.findAllByOrderByNameAsc();
            return this.message("Indexing Room", rooms, HttpStatus.OK);
        }
        List<Room> rooms = roomRepository.findAllByShape_idIsNullOrderByNameAsc();
        return this.message("Indexing Room", rooms, HttpStatus.OK);
    }

    @GetMapping("/{id}")
    public ResponseEntity<ResponseMessage> getById(@PathVariable UUID id) {
        log.debug("Getting User by id: {} ", id);
        Optional<Room> optionalRoom = this.roomRepository.findById(id);
        return optionalRoom.map(
                        order -> this.message("Getting Room by id", order, HttpStatus.OK))
                .orElseGet(() -> this.message("Room not found", null, HttpStatus.NOT_FOUND));
    }

    @PostMapping
    public ResponseEntity<ResponseMessage> create(@RequestBody Room room) {
        log.debug("create room: {}", room);
        Optional<Room> optionalRoom = (room.getId() != null) ? this.roomRepository.findById(room.getId()) : Optional.empty();

        if (optionalRoom.isPresent() && roomRepository.existsByName(room.getName())) {
            return this.message("Room is already exists", null, HttpStatus.CONFLICT);
        }
        Room roomCreated = this.roomRepository.save(room);
        List<Sensor> sensors = roomCreated.getSensors().stream().collect(Collectors.toList());
        ;
        roomCreated.getSensors().clear();
        for (Sensor sensor : sensors) {
            sensor.setRoom(roomCreated);
            this.sensorRepository.save(sensor);
        }
        return this.message("Created Room", roomCreated, HttpStatus.CREATED);
    }


    @PutMapping("/{id}")
    public ResponseEntity<ResponseMessage> update(@PathVariable("id") UUID id, @RequestBody Room room) {
        log.debug("Updating User by id: {}", id);
        Optional<Room> optionalRoom = this.roomRepository.findById(id);
        if (optionalRoom.isPresent() && optionalRoom.get().getId().equals(room.getId())) {
            Room roomUpdated = this.roomRepository.save(room);
            List<Sensor> sensors = room.getSensors().stream().collect(Collectors.toList());
            roomUpdated.getSensors().clear();
            for (Sensor sensor : sensors) {
                sensor.setRoom(roomUpdated);
                this.sensorRepository.save(sensor);
            }
            return this.message("Updating User by id", roomUpdated, HttpStatus.ACCEPTED);
        }
        return this.message("User not found", null, HttpStatus.NOT_FOUND);
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<ResponseMessage> delete(@PathVariable("id") UUID id) {
        log.debug("Deleting Room by id: {}", id);
        Optional<Room> roomUpdate = this.roomRepository.findById(id);

        if (roomUpdate.isPresent()) {
            this.roomRepository.deleteById(id);
            return this.message("Room is deleted", null, HttpStatus.NO_CONTENT);
        }
        return this.message("Room is not found", null, HttpStatus.NO_CONTENT);
    }

    @GetMapping("/{id}/sensors")
    public ResponseEntity<ResponseMessage> getSensors(@PathVariable("id") UUID id) {
        Room room = roomRepository.findById(id).orElse(null);
        if (room == null) {
            return ResponseEntity.notFound().build();
        }

        List<Sensor> sensors = room.getSensors();
        List<ValueMeasureDTO> values = new ArrayList<>();

        for (Sensor sensor : sensors) {
            List<ValueMeasure> sensorValues = sensor.getValueMeasures();
            if (!sensorValues.isEmpty()) {
                ValueMeasure lastValue = this.valueMeasureRepository.findAllBySensor_IdOrderByReadAtDesc(sensor.getId(), Limit.of(1)).get(0);
                ValueMeasureDTO lastValueDTO = new ValueMeasureDTO(null, lastValue.getValue(), lastValue.getReadAt(), lastValue.getCreatedAt(), lastValue.getUpdatedAt(),sensor.getType());
                values.add(lastValueDTO);
            }
        }
        BoardDTO boardDTO = new BoardDTO(sensors.get(0).getName().split("_")[0],values);
        return this.message("current values of Sensor",boardDTO,HttpStatus.OK);
    }

    private ResponseEntity<ResponseMessage> message(String message, Object data, HttpStatus httpStatus) {
        return new ResponseEntity<>(new ResponseMessage(message, data), httpStatus);
    }
}
