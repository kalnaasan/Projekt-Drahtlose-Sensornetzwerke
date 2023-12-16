package edu.fra.uas.interiorsensors.controller;

import edu.fra.uas.interiorsensors.common.ResponseMessage;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestParam;

import java.util.UUID;

public interface BaseController <T>{

    ResponseEntity<ResponseMessage> index(@RequestParam(defaultValue = "0") int page, @RequestParam(defaultValue = "0") int size);
    ResponseEntity<ResponseMessage> getById(UUID id);
    ResponseEntity<ResponseMessage> create(T t);
    ResponseEntity<ResponseMessage> update(UUID id , T t);
    ResponseEntity<ResponseMessage> delete(@PathVariable("id") UUID id);
}
