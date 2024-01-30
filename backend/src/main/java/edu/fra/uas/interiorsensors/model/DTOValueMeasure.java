package edu.fra.uas.interiorsensors.model;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import lombok.ToString;

import java.util.HashMap;
import java.util.Map;

@Data
@NoArgsConstructor
@AllArgsConstructor
@ToString
public class DTOValueMeasure {

    private String id;
    private Map<String, Integer> values = new HashMap<>();

}
