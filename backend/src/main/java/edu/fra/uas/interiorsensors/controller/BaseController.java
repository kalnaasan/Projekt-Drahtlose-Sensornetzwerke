package edu.fra.uas.interiorsensors.controller;

import edu.fra.uas.interiorsensors.common.ResponseMessage;
import org.springframework.http.ResponseEntity;

import java.util.UUID;

public interface BaseController<T> {
    ResponseEntity<ResponseMessage> index();

    ResponseEntity<ResponseMessage> getById(UUID id);

    ResponseEntity<ResponseMessage> create(T t);

    ResponseEntity<ResponseMessage> update(UUID id, T t);

    ResponseEntity<ResponseMessage> delete(UUID id);
}
