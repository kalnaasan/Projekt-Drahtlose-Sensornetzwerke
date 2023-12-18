package edu.fra.uas.interiorsensors.service;

import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import com.mbed.coap.packet.CoapResponse;
import com.mbed.coap.packet.Code;
import com.mbed.coap.packet.MediaTypes;
import com.mbed.coap.server.CoapServer;
import com.mbed.coap.server.RouterService;
import com.mbed.coap.server.observe.ObserversManager;
import com.mbed.coap.transport.udp.DatagramSocketTransport;

import lombok.extern.slf4j.Slf4j;

@Slf4j
@Service
public class CoAPServerImp {

    @Value("${server.host}")
    private String ipAddress;
    private CoapServer server;

    public CoAPServerImp() throws IllegalStateException, IOException {
        log.info("Starting CoAP server");
        // Geben Sie die IPv6-Adresse als String an
        String ipv6Address = "fd00:0000:fb01:0001::1";
        // Erstellen Sie ein InetAddress-Objekt
        InetAddress inetAddress = InetAddress.getByName(this.ipAddress);
        // Geben Sie den gewünschten Port an (z.B., 5683)
        int port = 5683;
        InetSocketAddress inetSocketAddress = new InetSocketAddress(inetAddress, port);
        ObserversManager observersManager = new ObserversManager();
        server = CoapServer.builder()
                .transport(new DatagramSocketTransport(inetSocketAddress))
                .route(
                        RouterService.builder()
                                .get("/.well-known/core",
                                        req -> CoapResponse
                                                .ok("</sensors/temperature>", MediaTypes.CT_APPLICATION_LINK__FORMAT)
                                                .toFuture())
                                .post("/actuators/switch",
                                        req -> CoapResponse.ok(Code.C204_CHANGED.codeToString(),
                                                MediaTypes.CT_APPLICATION_LINK__FORMAT).toFuture())
                                .get("/sensors/temperature",
                                        observersManager.then(req -> CoapResponse.ok("21C").toFuture()))
                                .post("/sensors/temperature", req -> {
                                    log.debug(req.toString());
                                    return CoapResponse.ok("confirmed").toFuture();
                                }))
                .build();
        observersManager.init(server);
        this.server.start();
        log.info("Server is Started with socket: coap:/{}", server.getLocalSocketAddress());
    }
}
