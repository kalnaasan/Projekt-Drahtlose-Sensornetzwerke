package edu.fra.uas.interiorsensors.repository;

import edu.fra.uas.interiorsensors.Classes.ValueMeasure;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;

import java.util.Optional;
import java.util.UUID;

public interface ValueMeasureRepository extends JpaRepository<ValueMeasure, UUID> {

    @Query("SELECT CASE WHEN COUNT(vm) > 0 THEN true ELSE false END FROM ValueMeasure vm WHERE vm.id = :id")
    boolean existsById(@Param("id") Long id);

    Optional<ValueMeasure> findByValue(double value); // Corrected method name
    boolean existsByValue(double value);

    Optional<ValueMeasure> findById(UUID id);
    boolean existsById(UUID id); // Corrected method name
}
