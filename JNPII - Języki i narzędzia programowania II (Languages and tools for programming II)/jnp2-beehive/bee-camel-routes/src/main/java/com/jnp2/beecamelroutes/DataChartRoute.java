package com.jnp2.beecamelroutes;

import org.apache.camel.builder.RouteBuilder;
import org.springframework.http.MediaType;
import org.springframework.stereotype.Component;

/*
    Provides an endpoint for obtaining a chart that
    presents data obtained from the beehive with a given id.
    (localhost:9093/chart/{id})

    Author: Sebastian Miller
 */

@Component
public class DataChartRoute extends RouteBuilder {
    @Override
    public void configure() throws Exception {
        restConfiguration().component("servlet").contextPath("/");

        rest("/chart")
            .get("/{id}")
            .produces(MediaType.IMAGE_PNG_VALUE)
            .route()
                .to("sql:SELECT * FROM measurements " +
                        "WHERE hive_id = :#id AND measured_at >= NOW() - 120000 " +
                        "ORDER BY measured_at DESC " +
                        "LIMIT 15") /* Query data */
                .process(new ChartAPIRequestCreator()) /* Prepare the request JSON. */
                .to("https://quickchart.io/chart?bridgeEndpoint=true") /* Send the JSON to Quickchart to obtain the chart image. */
            .endRest();
    }
}
