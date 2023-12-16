package edu.fra.uas.interiorsensors.repository;

import edu.fra.uas.interiorsensors.Classes.Room;
import edu.fra.uas.interiorsensors.Classes.Sensor;
import org.springframework.data.jpa.repository.JpaRepository;

import java.util.Optional;
import java.util.UUID;

public interface SensorRepository extends JpaRepository<Sensor,UUID> {

    Optional<Sensor> findByName(String name);
    boolean existsByName(String name);

    Optional<Sensor> findById(UUID id);
    boolean existsById(UUID id);

}