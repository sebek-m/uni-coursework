package com.jnp2.beecamelroutes;

import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.apache.camel.Exchange;
import org.apache.camel.Processor;

import java.util.HashMap;

/*
    This processor extracts the forecasted temperature from the weather forecast JSON.

    Author: Sebastian Miller
 */

public class TempExtractor implements Processor {

    @Override
    public void process(Exchange exchange) throws Exception {
        ObjectMapper mapper;
        String reportString;
        JsonNode report;
        int temp;

        mapper = new ObjectMapper();
        reportString = exchange.getIn().getBody(String.class);

        /*
         *  We're interested in the second position of the report,
         *  which corresponds to the weather expected in ~3 hours.
         */
        report = mapper.readTree(reportString).get("list").get(1).get("main");

        temp = report.get("temp").asInt();

        exchange.getIn().setHeader("temp", temp);
    }
}
