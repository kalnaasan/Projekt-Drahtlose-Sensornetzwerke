package edu.fra.uas.coap.server.service;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.mbed.coap.packet.CoapResponse;
import com.mbed.coap.packet.Code;
import com.mbed.coap.packet.MediaTypes;
import com.mbed.coap.server.CoapServer;
import com.mbed.coap.server.RouterService;
import com.mbed.coap.server.observe.ObserversManager;
import com.mbed.coap.transport.udp.DatagramSocketTransport;
import edu.fra.uas.coap.server.model.DTOValueMeasure;
import edu.fra.uas.coap.server.model.Sensor;
import edu.fra.uas.coap.server.model.ValueMeasure;
import edu.fra.uas.coap.server.repository.SensorRepository;
import edu.fra.uas.coap.server.repository.ValueMeasureRepository;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.time.LocalDateTime;
import java.util.Optional;

@Slf4j
@Service
public class CoAPServerService {
    @Value("${coap.server}")
    private String serverIP;
    private final SensorRepository sensorRepository;
    private final ValueMeasureRepository valueMeasureRepository;

    @Autowired
    public CoAPServerService(SensorRepository sensorRepository, ValueMeasureRepository valueMeasureRepository) throws IllegalStateException, IOException {
        this.sensorRepository = sensorRepository;
        this.valueMeasureRepository = valueMeasureRepository;

        log.info("Starting CoAP server");
        InetSocketAddress inetSocketAddress = new InetSocketAddress(this.serverIP, 5683);
        ObserversManager observersManager = new ObserversManager();
        CoapServer server = CoapServer.builder()
                .transport(new DatagramSocketTransport(inetSocketAddress))
                .route(RouterService.builder()
                        .get("/.well-known/core",
                                req -> CoapResponse.ok("</sensors/temperature>", MediaTypes.CT_APPLICATION_LINK__FORMAT).toFuture())
                        .post("/actuators/switch",
                                req -> CoapResponse.ok(Code.C204_CHANGED.codeToString(), MediaTypes.CT_APPLICATION_LINK__FORMAT).toFuture())
                        .get("/sensors/temperature",
                                observersManager.then(req -> CoapResponse.ok("21C").toFuture()))
                        .put("/sensors", req -> {
                            log.debug("Adding values to Sensor");
                            String json = new String(req.getPayload().getBytes());
                            log.debug("JSON => {}", json);

                            DTOValueMeasure valueMeasure = convertJSONToObject(json);
                            assert valueMeasure != null;
                            for (String key : valueMeasure.getValues().keySet()) {
                                String name = valueMeasure.getId() + '_' + key;

                                Optional<Sensor> sensorOptional = this.sensorRepository.findByName(name);
                                Sensor sensor;
                                if (sensorOptional.isEmpty()) {
                                    // create sensor
                                    sensor = new Sensor();
                                    sensor.setName(name);
                                    sensor.setType(key.split("_")[1]);
                                    sensor = this.sensorRepository.save(sensor);
                                } else {
                                    sensor = sensorOptional.get();
                                }
                                // create value for the sensor
                                ValueMeasure value = new ValueMeasure();
                                value.setValue(valueMeasure.getValues().get(key));
                                value.setReadAt(LocalDateTime.now());
                                value.setSensor(sensor);
                                this.valueMeasureRepository.save(value);
                            }
                            return CoapResponse.ok("confirmed").toFuture();
                        })).build();
        observersManager.init(server);
        server.start();
        log.info("Server is Started with socket: coap:/{}", server.getLocalSocketAddress());
    }

    private static DTOValueMeasure convertJSONToObject(String json) {
        // ObjectMapper erstellen
        ObjectMapper objectMapper = new ObjectMapper();
        try {
            // JSON-String in DTOValueMeasure umwandeln
            DTOValueMeasure dtoValueMeasure = objectMapper.readValue(json, DTOValueMeasure.class);
            // Ausgabe des Java-Objekts
            log.info(dtoValueMeasure.toString());
            return dtoValueMeasure;
        } catch (Exception e) {
            log.error(e.getMessage());
            return null;
        }
    }
}
