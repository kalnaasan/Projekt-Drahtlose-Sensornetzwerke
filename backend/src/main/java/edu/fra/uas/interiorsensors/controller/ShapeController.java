package edu.fra.uas.interiorsensors.controller;

import edu.fra.uas.interiorsensors.common.ResponseMessage;
import edu.fra.uas.interiorsensors.model.Element;
import edu.fra.uas.interiorsensors.model.Room;
import edu.fra.uas.interiorsensors.model.Shape;
import edu.fra.uas.interiorsensors.repository.ElementRepository;
import edu.fra.uas.interiorsensors.repository.RoomRepository;
import edu.fra.uas.interiorsensors.repository.ShapeRepository;
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
@RequestMapping("shapes")
public class ShapeController {
    private final ShapeRepository shapeRepository;
    private final ElementRepository elementRepository;
    private final RoomRepository roomRepository;

    @Autowired
    public ShapeController(ShapeRepository shapeRepository, ElementRepository elementRepository, RoomRepository roomRepository) {
        this.shapeRepository = shapeRepository;
        this.elementRepository = elementRepository;
        this.roomRepository = roomRepository;
    }

    @GetMapping
    public ResponseEntity<ResponseMessage> index() {
        log.debug("Indexing Shape : {}", this.shapeRepository);
        return this.message("Indexing Shape", this.shapeRepository.findAll(), HttpStatus.OK);
    }

    @GetMapping("/{id}")
    public ResponseEntity<ResponseMessage> getById(@PathVariable UUID id) {
        log.debug("Getting Shape by id: {} ", id);
        Optional<Shape> optionalShape = this.shapeRepository.findById(id);
        return optionalShape.map(
                        shape -> this.message("Getting Shape by id", shape, HttpStatus.OK))
                .orElseGet(() -> this.message("Shape not found", null, HttpStatus.NOT_FOUND));
    }

    @PostMapping
    public ResponseEntity<ResponseMessage> create(@RequestBody Shape shape) {
        log.debug("create Shape: {}", shape);
        Optional<Shape> optionalShape = (shape.getId() != null) ? this.shapeRepository.findById(shape.getId()) : Optional.empty();

        if (optionalShape.isPresent()) {
            return this.message("Shape is already exists", null, HttpStatus.CONFLICT);
        }
        List<Room> rooms = shape.getRooms().stream().toList();
        shape.getRooms().clear();

        List<Element> elements = shape.getElements().stream().toList();
        shape.getElements().clear();

        Shape shapeCreated = this.shapeRepository.save(shape);

        for (Room room : rooms) {
            room.setShape(shapeCreated);
            this.roomRepository.save(room);
        }

        for (Element element : elements) {
            Room room = null;
            if (element.getRoom() != null){
                room = this.roomRepository.findById(element.getRoom().getId()).orElseGet(null);
            }
            element.setRoom(room);
            element.setShape(shapeCreated);
            this.elementRepository.save(element);
        }

        return this.message("Created Shape", null, HttpStatus.CREATED);
    }


    @PutMapping("/{id}")
    public ResponseEntity<ResponseMessage> update(@PathVariable("id") UUID id, @RequestBody Shape shape) {
        log.debug("Updating Shape by id: {}", id);
        Optional<Shape> optionalShape = this.shapeRepository.findById(id);
        if (optionalShape.isPresent() && optionalShape.get().getId().equals(shape.getId())) {
            // remove the shape fro rooms
            for (Room room : optionalShape.get().getRooms()) {
                room.setShape(null);
                this.roomRepository.save(room);
            }
            // remove the shape from the elements
            this.elementRepository.deleteAll(optionalShape.get().getElements());

            // copy the rooms of the shape
            List<Room> rooms = shape.getRooms().stream().toList();
            shape.getRooms().clear();
            // copy the elements of the shape
            List<Element> elements = shape.getElements().stream().toList();
            shape.getElements().clear();
            // update the shape
            Shape shapeCreated = this.shapeRepository.save(shape);
            // update the rooms
            for (Room room : rooms) {
                room.setShape(shapeCreated);
                this.roomRepository.save(room);
            }
            // update the elements
            for (Element element : elements) {
                Room room = null;
                if (element.getRoom() != null){
                    room = this.roomRepository.findById(element.getRoom().getId()).orElseGet(null);
                }
                element.setRoom(room);
                element.setShape(shapeCreated);
                this.elementRepository.save(element);
            }
            return this.message("Updating shape", null, HttpStatus.ACCEPTED);
        }
        return this.message("Shape not found", null, HttpStatus.NOT_FOUND);
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<ResponseMessage> delete(@PathVariable("id") UUID id) {
        log.debug("Deleting Shape by id: {}", id);
        Optional<Shape> roomUpdate = this.shapeRepository.findById(id);

        if (roomUpdate.isPresent()) {
            this.shapeRepository.deleteById(id);
            return this.message("Shape is deleted", null, HttpStatus.NO_CONTENT);
        }
        return this.message("Shape is not found", null, HttpStatus.NO_CONTENT);
    }

    private ResponseEntity<ResponseMessage> message(String message, Object data, HttpStatus httpStatus) {
        return new ResponseEntity<>(new ResponseMessage(message, data), httpStatus);
    }
}
