package edu.fra.uas.interiorsensors.repository;

import org.springframework.data.jpa.repository.JpaRepository;

import edu.fra.uas.interiorsensors.model.Sensor;

import java.util.Optional;
import java.util.UUID;

public interface SensorRepository extends JpaRepository<Sensor,UUID> {

    Optional<Sensor> findByName(String name);
    boolean existsByName(String name);

    Optional<Sensor> findById(UUID id);
    boolean existsById(UUID id);

}
