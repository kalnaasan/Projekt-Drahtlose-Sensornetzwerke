package edu.fra.uas.interiorsensors.repository;

import edu.fra.uas.interiorsensors.model.ValueMeasure;
import org.springframework.data.domain.Limit;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.stereotype.Repository;

import java.time.LocalDateTime;
import java.util.List;
import java.util.UUID;

@Repository
public interface ValueMeasureRepository extends JpaRepository<ValueMeasure, UUID> {
    boolean existsByValue(double value);
    List<ValueMeasure> findAllBySensor_IdOrderByReadAtDesc(UUID sensorId, Limit limit);
    List<ValueMeasure> findAllBySensor_NameAndReadAtBetweenOrderByReadAtAsc(String sensor_name, LocalDateTime start, LocalDateTime end);

    @Query("SELECT HOUR(vm.readAt) AS readAt, AVG(vm.value) AS value FROM ValueMeasure vm LEFT JOIN Sensor s ON s.id = vm.sensor.id Where s.name=:sensorName AND vm.readAt>=:start AND vm.readAt<=:end GROUP BY HOUR(vm.readAt) ORDER BY HOUR (vm.readAt)")
    List<Object[]> findAllBySensor_NameAndReadAtBetweenAndAverageReadAtOrderByReadAt(String sensorName, LocalDateTime start, LocalDateTime end);

    @Query(value = "SELECT HOUR(vm.read_at) DIV 4 as period, AVG(vm.value) as averageValue FROM value_measure vm JOIN sensors s WHERE s.name = ?1 AND vm.read_at BETWEEN ?2 AND ?3 GROUP BY period ORDER BY period", nativeQuery = true)
    List<Object[]> findAllBySensor_NameAndReadAtBetweenAndAverageByFourHours_ReadAtOrderByReadAt(String sensorName, LocalDateTime from, LocalDateTime to);
}
