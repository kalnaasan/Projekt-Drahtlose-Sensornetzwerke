package edu.fra.uas.interiorsensors.controller;


import edu.fra.uas.interiorsensors.Classes.Room;
import edu.fra.uas.interiorsensors.common.ResponseMessage;
import edu.fra.uas.interiorsensors.repository.RoomRepository;

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
@RequestMapping("/api/v1/web/rooms")
public class RoomController {

    private final RoomRepository roomRepository;

    private final Logger logger = LoggerFactory.getLogger(RoomRepository.class);

    @Autowired
    public RoomController(RoomRepository roomRepository) {
        this.roomRepository = roomRepository;
    }

    @GetMapping
    public ResponseEntity<ResponseMessage> index(@RequestParam(defaultValue = "0") int page, @RequestParam(defaultValue = "10") int size){
        logger.debug("Indexing Room : {}",this.roomRepository);
        List<Room> rooms = roomRepository.findAll();
        return this.message("Indexing Room",rooms,HttpStatus.OK);
    }

    @GetMapping("/{id}")
    public ResponseEntity<ResponseMessage> getById(@PathVariable UUID id) {
        logger.debug("Getting User by id: {} ", id);
        Optional<Room> optionalRoom = this.roomRepository.findById(id);
        return optionalRoom.map(
                        order -> this.message("Getting Room by id", order, HttpStatus.OK))
                .orElseGet(() -> this.message("Room not found", null, HttpStatus.NOT_FOUND));
    }

    @PostMapping
    public ResponseEntity<ResponseMessage> create(@RequestBody Room room){
        logger.debug("create room: {}", room);
        Optional<Room> optionalRoom = (room.getId()!= null)? this.roomRepository.findById(room.getId()) : Optional.empty();

        if (optionalRoom.isPresent() && roomRepository.existsByName(room.getName())){
            return this.message("Room is already exists", null, HttpStatus.CONFLICT);
        }
        Room roomCreated = this.roomRepository.save(room);
        return this.message("Created Room",roomCreated,HttpStatus.CREATED);
    }

    @PutMapping("/{id}")
    public ResponseEntity<ResponseMessage> update(@PathVariable("id") UUID id, @RequestBody Room room) {
        logger.debug("Updating User by id: {}", id);
        Optional<Room> optionalRoom = this.roomRepository.findById(id);
        if (optionalRoom.isPresent() && optionalRoom.get().getId().equals(room.getId())) {

            Room roomCreated = this.roomRepository.save(room);

            return this.message("Updating User by id", roomCreated, HttpStatus.ACCEPTED);
        }
        return this.message("User not found", null, HttpStatus.NOT_FOUND);
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<ResponseMessage> delete(@PathVariable("id") UUID id){
        logger.debug("Deleting Room by id: {}", id);
        Optional<Room> roomUpdate = this.roomRepository.findById(id);

        if (roomUpdate.isPresent()){
            this.roomRepository.deleteById(id);
            return this.message("Room is deleted", null , HttpStatus.NO_CONTENT);
        }
        return this.message("Room is not found", null , HttpStatus.NO_CONTENT);
    }



    private ResponseEntity<ResponseMessage> message(String message, Object data, HttpStatus httpStatus) {
        return new ResponseEntity<>(new ResponseMessage(message, data), httpStatus);
    }
}
