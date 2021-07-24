package com.jnp2.beehivesimulation;

import org.apache.camel.builder.RouteBuilder;
import org.springframework.stereotype.Component;

/*
    Consumes tasks from the 'tasks' Kafka topic
    and simulates an appropriate reaction to the
    task by logging the action taken.

    Author: Sebastian Miller
 */

@Component
public class TaskConsumptionRoute extends RouteBuilder {
    @Override
    public void configure() throws Exception {
        from("kafka:tasks?brokers=kafka:29092")
                /* Consume either an external, universal task with id = 0
                    or a task published by the measurement module.
                 */
                .filter(simple("${headers[kafka.KEY]} == 0 || " +
                               "${headers[kafka.KEY]} == ${properties:beehive.id}"))
                .choice()
                    .when(simple("${body} == 'close'"))
                        .log("Closing entrance...")
                    .when(simple("${body} == 'open'"))
                        .log("Opening entrance...")
                    .otherwise()
                        .log("Invalid task received!");
    }
}
