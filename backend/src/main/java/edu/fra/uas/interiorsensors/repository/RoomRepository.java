package edu.fra.uas.interiorsensors.repository;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import edu.fra.uas.interiorsensors.model.Room;

import java.util.Optional;
import java.util.UUID;

@Repository
public interface RoomRepository extends JpaRepository<Room, UUID> {
    Optional<Room> findByName(String name);

    boolean existsByName(String name);

    Optional<Room> findById(UUID id);

    boolean existsById(UUID id);
}
