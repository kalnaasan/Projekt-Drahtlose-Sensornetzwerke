package edu.fra.uas.interiorsensors.model;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.util.HashMap;
import java.util.Map;

@Data
@NoArgsConstructor
@AllArgsConstructor
public class DTOValueMeasure {

    private Integer id;
   private Map<String , Integer> values = new HashMap<>();



}
