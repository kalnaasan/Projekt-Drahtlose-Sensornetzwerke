package edu.frauas.coapserver;

import java.io.IOException;
import java.net.InetSocketAddress;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;

import com.mbed.coap.packet.CoapResponse;
import com.mbed.coap.packet.Code;
import com.mbed.coap.packet.MediaTypes;
import com.mbed.coap.server.CoapServer;
import com.mbed.coap.server.RouterService;
import com.mbed.coap.server.observe.ObserversManager;
import com.mbed.coap.transport.udp.DatagramSocketTransport;

@Service
public class Server {
    private static final Logger LOG = LoggerFactory.getLogger(Server.class);
    private final CoapServer server;

    public Server() throws IllegalStateException, IOException {
        LOG.info("Starting CoAP server");
        InetSocketAddress inetSocketAddress = new InetSocketAddress("127.0.0.1", 5683);
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
                                    LOG.debug(req.toString());
                                    return CoapResponse.ok("confirmed").toFuture();
                                }))
                .build();
        observersManager.init(server);
        this.server.start();
        LOG.info("Server is Started with socket: coap:/{}", server.getLocalSocketAddress());
    }
}
