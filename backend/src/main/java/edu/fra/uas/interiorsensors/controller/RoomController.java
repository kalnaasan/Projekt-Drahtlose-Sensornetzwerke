package edu.fra.uas.interiorsensors.controller;


import edu.fra.uas.interiorsensors.common.ResponseMessage;
import edu.fra.uas.interiorsensors.model.Room;
import edu.fra.uas.interiorsensors.repository.RoomRepository;

import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.CrossOrigin;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.PutMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;
import java.util.Optional;
import java.util.UUID;

@Slf4j
@CrossOrigin(origins = "*")
@RestController
@RequestMapping("rooms")
public class RoomController implements BaseController<Room> {

    private final RoomRepository roomRepository;

    @Autowired
    public RoomController(RoomRepository roomRepository) {
        this.roomRepository = roomRepository;
    }

    @GetMapping
    @Override
    public ResponseEntity<ResponseMessage> index() {
        log.debug("Indexing Room : {}", this.roomRepository);
        List<Room> rooms = roomRepository.findAll();
        return this.message("Indexing Room", rooms, HttpStatus.OK);
    }

    @GetMapping("/{id}")
    @Override
    public ResponseEntity<ResponseMessage> getById(@PathVariable UUID id) {
        log.debug("Getting User by id: {} ", id);
        Optional<Room> optionalRoom = this.roomRepository.findById(id);
        return optionalRoom.map(
                        order -> this.message("Getting Room by id", order, HttpStatus.OK))
                .orElseGet(() -> this.message("Room not found", null, HttpStatus.NOT_FOUND));
    }

    @PostMapping
    @Override
    public ResponseEntity<ResponseMessage> create(@RequestBody Room room) {
        log.debug("create room: {}", room);
        Optional<Room> optionalRoom = (room.getId() != null) ? this.roomRepository.findById(room.getId()) : Optional.empty();

        if (optionalRoom.isPresent() && roomRepository.existsByName(room.getName())) {
            return this.message("Room is already exists", null, HttpStatus.CONFLICT);
        }
        Room roomCreated = this.roomRepository.save(room);
        return this.message("Created Room", roomCreated, HttpStatus.CREATED);
    }

    @PutMapping("/{id}")
    @Override
    public ResponseEntity<ResponseMessage> update(@PathVariable("id") UUID id, @RequestBody Room room) {
        log.debug("Updating User by id: {}", id);
        Optional<Room> optionalRoom = this.roomRepository.findById(id);
        if (optionalRoom.isPresent() && optionalRoom.get().getId().equals(room.getId())) {

            Room roomCreated = this.roomRepository.save(room);

            return this.message("Updating User by id", roomCreated, HttpStatus.ACCEPTED);
        }
        return this.message("User not found", null, HttpStatus.NOT_FOUND);
    }

    @DeleteMapping("/{id}")
    @Override
    public ResponseEntity<ResponseMessage> delete(@PathVariable("id") UUID id) {
        log.debug("Deleting Room by id: {}", id);
        Optional<Room> roomUpdate = this.roomRepository.findById(id);

        if (roomUpdate.isPresent()) {
            this.roomRepository.deleteById(id);
            return this.message("Room is deleted", null, HttpStatus.NO_CONTENT);
        }
        return this.message("Room is not found", null, HttpStatus.NO_CONTENT);
    }

    private ResponseEntity<ResponseMessage> message(String message, Object data, HttpStatus httpStatus) {
        return new ResponseEntity<>(new ResponseMessage(message, data), httpStatus);
    }
}
