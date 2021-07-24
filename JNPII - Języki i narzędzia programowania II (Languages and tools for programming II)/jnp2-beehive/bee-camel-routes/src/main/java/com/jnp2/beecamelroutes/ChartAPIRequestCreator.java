package com.jnp2.beecamelroutes;

import com.fasterxml.jackson.databind.ObjectMapper;
import org.apache.camel.Exchange;
import org.apache.camel.Processor;

import java.util.List;
import java.util.Map;
import java.sql.Timestamp;
import java.util.Date;
import java.text.SimpleDateFormat;

/*
    This processor extracts data from the SQL query results
    and prepares an appropriate request with a JSON to the
    Quickchart.io API.

    Author: Sebastian Miller
 */
public class ChartAPIRequestCreator implements Processor {
    /* Supplement classes, resembling the Quickchart JSON structure */
    private static class ChartConfig {
        public final String type = "line";
        public ChartData data;
    }

    private static class ChartData {
        public String[] labels;
        public ChartDataset[] datasets;
    }

    private static class ChartDataset {
        public String label;
        public int[] data;
        public String borderColor;

        public final boolean fill = false;
    }

    @Override
    public void process(Exchange exchange) throws Exception {
        /* Get the SQL query results. */
        List<Map<String, Object>> results = exchange.getIn().getBody(List.class);

        /* Prepare the objects with data. */
        ChartDataset temp = new ChartDataset();
        temp.label = "Temperature";
        temp.data = new int[results.size()];
        temp.borderColor = "red";

        ChartDataset humidity = new ChartDataset();
        humidity.label = "Humidity";
        humidity.data = new int[results.size()];
        humidity.borderColor = "blue";

        ChartData data = new ChartData();
        data.labels = new String[results.size()];
        data.datasets = new ChartDataset[]{temp, humidity};

        /* Rewrite data to the objects. */
        for (int i = results.size() - 1; i >= 0; i--) {
            Map<String, Object> row = results.get(results.size() - i - 1);

            temp.data[i] = (int) row.get("temperature");
            humidity.data[i] = (int) row.get("humidity");

            Timestamp timestamp = (Timestamp) row.get("measured_at");
            Date date = new Date(timestamp.getTime());
            SimpleDateFormat sdf = new SimpleDateFormat("hh:mm:ss");
            data.labels[i] = sdf.format(date);
        }

        ChartConfig config = new ChartConfig();
        config.data = data;

        /* Provide the chart config JSON in the 'c' query, as specified in Quickchart API documentation. */
        ObjectMapper mapper = new ObjectMapper();
        exchange.getIn().setHeader(Exchange.HTTP_QUERY, "c=" + mapper.writeValueAsString(config));
        exchange.getIn().setBody(null);
    }
}
