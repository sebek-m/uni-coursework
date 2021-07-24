package com.jnp2.beecamelroutes;

import org.apache.camel.Exchange;
import org.apache.camel.Processor;
import org.apache.camel.builder.RouteBuilder;
import org.apache.camel.component.kafka.KafkaConstants;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

/*
    Cyclically queries the OpenWeather API for weather forecast
    and produces appropriate tasks according to the forecasted temperature.

    Author: Sebastian Miller
 */

@Component
public class WeatherForecastRoute extends RouteBuilder {
    /*
        We do not know whether particular beehive entrances
        are open or closed, since the state may be changed individually
        according to internal metrics, but we still want to
        order a change only when necessary.
     */
    private boolean expectedClosed = false;

    /*
        Temperature may either rise above or drop below
        the threshold and, depending on which of the two
        has happened, an appropriate task is published (or
        no task is published if temperature remains above/
        below the threshold).
     */
    @Value("${temp.threshold}")
    private int threshold;

    @Override
    public void configure() throws Exception {
        from("weather:w1?location={{weather.city}}" +
                           "&appid={{weather.key}}" +
                           "&delay={{weather.interval}}" +
                           "&weatherApi=Hourly" +
                           "&units=METRIC&period=5 days") /* Obtain the forecast. */
                .process(new TempExtractor()) /* Extract the temperature. */
                .log("Forecasted temperature: ${header.temp}")
                .process(new Processor() {
                    @Override
                    public void process(Exchange exchange) throws Exception {
                        /*
                            Decide whether and which task should be sent and update
                            the expected entrance status.
                         */
                        int temp = exchange.getIn().getHeader("temp", Integer.class);

                        exchange.getIn().setHeader("send", true);

                        if (temp < threshold && !expectedClosed) {
                            expectedClosed = true;
                            exchange.getIn().setBody("close");
                        }
                        else if (temp >= threshold && expectedClosed) {
                            expectedClosed = false;
                            exchange.getIn().setBody("open");
                        }
                        else {
                            exchange.getIn().setHeader("send", false);
                        }
                    }
                })
                .filter(simple("${header.send} == true"))
                .to("direct://publishTask");

        from("direct://publishTask")
                .setHeader(KafkaConstants.KEY, constant(0)) /* Universal beehive id */
                .to("kafka:tasks?brokers=kafka:29092")
                .log("Published task: ${body}");
    }
}
