package edu.fra.uas.interiorsensors.model;

import lombok.*;

import java.util.List;
@Data
@NoArgsConstructor
@AllArgsConstructor
@ToString
@EqualsAndHashCode(exclude = "valueMeasures")
public class BoardDTO {
    private String name;
    private List<ValueMeasureDTO> valueMeasures;
}
