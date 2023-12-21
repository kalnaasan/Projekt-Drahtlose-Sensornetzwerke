package edu.fra.uas.coap.server.repository;

import edu.fra.uas.coap.server.model.ValueMeasure;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.UUID;

@Repository
public interface ValueMeasureRepository extends JpaRepository<ValueMeasure, UUID> {
}
