package edu.fra.uas.interiorsensors.model;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.EqualsAndHashCode;
import lombok.NoArgsConstructor;
import lombok.ToString;

import java.util.List;

@Data
@NoArgsConstructor
@AllArgsConstructor
@ToString
@EqualsAndHashCode(exclude = "valueMeasures")
public class BoardDTO {
    private String name;
    private String roomName;
    private List<ValueMeasureDTO> valueMeasures;
}
