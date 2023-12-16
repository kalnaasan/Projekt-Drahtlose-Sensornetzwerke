package edu.fra.uas.interiorsensors.Classes;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import lombok.ToString;
import org.springframework.boot.autoconfigure.domain.EntityScan;

import javax.persistence.*;
import java.util.UUID;

@Data
@NoArgsConstructor
@AllArgsConstructor
@ToString
@Entity
@Table(name = "Value_measure")
public class ValueMeasure {
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private UUID id;

    @Column
    private double value;

    @ManyToOne(targetEntity = Sensor.class)
    @JoinColumn(name = "sensor_id")
    private Sensor sensor;
}


