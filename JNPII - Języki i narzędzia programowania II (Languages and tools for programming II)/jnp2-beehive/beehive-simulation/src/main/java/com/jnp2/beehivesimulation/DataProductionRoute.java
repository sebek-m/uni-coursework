package com.jnp2.beehivesimulation;

import org.apache.camel.Exchange;
import org.apache.camel.Processor;
import org.apache.camel.builder.RouteBuilder;
import org.apache.camel.component.kafka.KafkaConstants;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

import java.util.Random;

/*
    Generates random temperature and humidity data, thus simulating
    the internal beehive status, publishes the data, and potentially
    publishes an appropriate task for the entrance module.

    Author: Sebastian Miller
 */

@Component
public class DataProductionRoute extends RouteBuilder {
    private final int MIN_TEMP = 15;
    private final int TEMP_RANGE = 40;
    private final int MIN_HUMIDITY = 15;
    private final int HUMIDITY_RANGE = 60;

    private Random generator = new Random();

    /*
        The expectedClosed and threshold values
        are analogous to the ones in the WeatherForecastRoute
        but they correspond to the internal beehive status
        and are independent of weather forecast.
     */
    private boolean expectedClosed = false;

    @Value("${threshold.temp}")
    private int tempThreshold;

    @Value("${threshold.humidity}")
    private int humidityThreshold;

    /* Helper class for JSON marshalling. */
    private static class DataFormat {
        private int temperature;
        private int humidity;

        public DataFormat(int temperature, int humidity) {
            this.temperature = temperature;
            this.humidity = humidity;
        }

        public int getTemperature() {
            return this.temperature;
        }

        public int getHumidity() {
            return this.humidity;
        }
    }

    @Override
    public void configure() throws Exception {
        from("timer:t1?period=10000")
                .setHeader(KafkaConstants.KEY, simple("${properties:beehive.id}"))
                .process(new Processor() {
                    @Override
                    public void process(Exchange exchange) throws Exception {
                        /* Generate internal parameters and prepare message. */
                        int temp = MIN_TEMP + generator.nextInt(TEMP_RANGE);
                        int humidity = MIN_HUMIDITY + generator.nextInt(HUMIDITY_RANGE);

                        exchange.getIn().setHeader("send", true);

                        if (expectedClosed && (temp >= tempThreshold || humidity >= humidityThreshold)) {
                            exchange.getIn().setHeader("task", "open");
                            expectedClosed = false;
                        }
                        else if (!expectedClosed && (temp < tempThreshold || humidity < humidityThreshold)) {
                            exchange.getIn().setHeader("task", "close");
                            expectedClosed = true;
                        }
                        else {
                            exchange.getIn().setHeader("send", false);
                        }

                        exchange.getIn().setBody(new DataFormat(temp, humidity));
                    }
                })
                .log("Measurements -- Temperature: ${body.temperature}C, Humidity: ${body.humidity}%")
                .multicast().to("direct:taskTarget", "direct:dataTarget");


        /* Maybe publish an appropriate task. */
        from("direct:taskTarget")
                .filter(simple("${header.send} == true"))
                .setBody(simple("${header.task}"))
                .to("kafka:tasks?brokers=kafka:29092&valueSerializer=org.apache.kafka.common.serialization.StringSerializer");

        /* Publish the generated data. */
        from("direct:dataTarget")
                .marshal().json()
                .to("kafka:data?brokers=kafka:29092");
    }
}
