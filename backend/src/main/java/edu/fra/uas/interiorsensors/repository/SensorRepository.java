package edu.fra.uas.interiorsensors.repository;

import edu.fra.uas.interiorsensors.model.Sensor;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Optional;
import java.util.UUID;

@Repository
public interface SensorRepository extends JpaRepository<Sensor, UUID> {
    Optional<Sensor> findByName(String name);

    List<Sensor> findAllByRoom_IdOrderByName(UUID roomId);
    List<Sensor> findAllByRoomIsNullOrderByName();

    boolean existsByName(String name);
}
