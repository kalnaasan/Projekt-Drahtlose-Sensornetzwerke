package edu.fra.uas.interiorsensors.repository;


import edu.fra.uas.interiorsensors.model.Room;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.UUID;

@Repository
public interface RoomRepository extends JpaRepository<Room, UUID> {
    boolean existsByName(String name);

    List<Room> findAllByShapeIsNullOrderByNameAsc();

    List<Room> findAllByOrderByNameAsc();
}
