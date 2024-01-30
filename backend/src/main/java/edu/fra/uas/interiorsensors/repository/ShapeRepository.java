package edu.fra.uas.interiorsensors.repository;

import edu.fra.uas.interiorsensors.model.Shape;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.UUID;

@Repository
public interface ShapeRepository extends JpaRepository<Shape, UUID> {
}
