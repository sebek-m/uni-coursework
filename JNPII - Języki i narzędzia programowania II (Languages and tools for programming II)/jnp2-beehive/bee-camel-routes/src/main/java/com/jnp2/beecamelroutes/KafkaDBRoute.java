package com.jnp2.beecamelroutes;

import org.apache.camel.Exchange;
import org.apache.camel.Processor;
import org.apache.camel.builder.RouteBuilder;
import org.apache.camel.component.kafka.KafkaConstants;
import org.apache.camel.model.dataformat.JsonLibrary;
import org.springframework.stereotype.Component;

/*
    Consumes measurements from the 'data' Kafka topic
    and transfers them to the database.

    Author: Sebastian Miller
 */

@Component
public class KafkaDBRoute extends RouteBuilder {
    /* Helper class for JSON unmarshalling. */
    private static class BeehiveData {
        private int temperature;
        private int humidity;

        public BeehiveData(int temperature, int humidity) {
            this.temperature = temperature;
            this.humidity = humidity;
        }
        
        public BeehiveData() {}

        public int getTemperature() {
            return temperature;
        }

        public int getHumidity() {
            return humidity;
        }
        
        public void setTemperature(int temperature) {
            this.temperature = temperature;
        }
        
        public void setHumidity(int humidity) {
            this.humidity = humidity;
        }
    }

    @Override
    public void configure() throws Exception {
        from("kafka:data?brokers=kafka:29092")
                .unmarshal()
                .json(JsonLibrary.Jackson, BeehiveData.class)
                .process(new Processor() {
                    @Override
                    public void process(Exchange exchange) throws Exception {
                        int hiveId = exchange.getIn().getHeader(KafkaConstants.KEY, Integer.class);
                        BeehiveData data = exchange.getIn().getBody(BeehiveData.class);

                        long timestamp = exchange.getIn().getHeader(KafkaConstants.TIMESTAMP, Long.class);

                        exchange.getIn().setHeader("hive_id", hiveId);
                        exchange.getIn().setHeader("temp", data.getTemperature());
                        exchange.getIn().setHeader("humidity", data.getHumidity());
                        
                        /* Convert to second precision, required by the 'from_unixtime' MySQL function. */
                        exchange.getIn().setHeader("timestamp", timestamp / 1000);
                    }
                })
                .to("sql:INSERT INTO measurements VALUES(:#hive_id, :#temp, :#humidity, from_unixtime(:#timestamp))");
    }
}
