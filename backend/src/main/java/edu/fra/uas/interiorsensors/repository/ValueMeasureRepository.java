package edu.fra.uas.interiorsensors.repository;

import edu.fra.uas.interiorsensors.model.ValueMeasure;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.UUID;

@Repository
public interface ValueMeasureRepository extends JpaRepository<ValueMeasure, UUID> {
    boolean existsByValue(double value);
}
