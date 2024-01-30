package edu.fra.uas.coap.server.model;

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
    private List<ValueMeasureDTO> valueMeasures;
}
