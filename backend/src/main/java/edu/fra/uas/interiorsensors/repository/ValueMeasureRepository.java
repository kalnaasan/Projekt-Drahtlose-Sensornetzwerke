package edu.fra.uas.interiorsensors.repository;

import edu.fra.uas.interiorsensors.model.ValueMeasure;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.time.LocalDateTime;
import java.util.List;
import java.util.UUID;

@Repository
public interface ValueMeasureRepository extends JpaRepository<ValueMeasure, UUID> {
    boolean existsByValue(double value);

    List<ValueMeasure> findAllBySensor_NameAndReadAtBetweenOrderByCreatedAtAsc(String sensor_name, LocalDateTime start, LocalDateTime end);
}
