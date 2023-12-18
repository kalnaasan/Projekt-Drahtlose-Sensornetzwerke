package edu.fra.uas.interiorsensors.Classes;

import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.*;
import org.springframework.boot.autoconfigure.domain.EntityScan;

import javax.persistence.*;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

@Data
@NoArgsConstructor
@AllArgsConstructor
@ToString
@EqualsAndHashCode(exclude = {"sensors"})
@Entity
@Table(name = "rooms")
public class Room {

    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private UUID id;

    @Column(name = "name")
    @JsonProperty("name")
    private String name;

    @OneToMany(targetEntity = Sensor.class,mappedBy = "room", cascade = CascadeType.MERGE)
    private List<Sensor> sensors = new ArrayList<>();
}