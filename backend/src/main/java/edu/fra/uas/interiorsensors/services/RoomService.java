package edu.fra.uas.interiorsensors.services;

import jakarta.persistence.Entity;
import jakarta.persistence.EntityManager;
import jakarta.transaction.Transactional;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
public class RoomService {

    private final EntityManager entityManager;

    @Autowired
    public RoomService(EntityManager entityManager) {
        this.entityManager = entityManager;
    }

    @Transactional
    public void deineMethode(Entity entity) {
        entityManager.persist(entity);
    }

}
