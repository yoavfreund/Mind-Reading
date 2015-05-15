$(document).ready(function() {
    //Width and height
    var w = 1000;
    var h = 100;

    var steps=50
    var dataset = [ 0,0 ]


    var yScale = d3.scale.ordinal()
	.domain(d3.range(dataset.length*2+1))
	.rangeRoundBands([0, h], 0.05);

    var xScale = d3.scale.linear()
	.domain([0, steps+2])
	.range([0, w]);

    //Create SVG element
    var svg = d3.select("body")
	.append("svg")
	.attr("width", w)
	.attr("height", h)
	.attr("fill","green");

    // draw tracks

    var track1=svg.append("line")
	.attr("x1", xScale(2))
	.attr("y1", yScale(1))
	.attr("x2", xScale(steps+1))
	.attr("y2", yScale(1))
	.attr("stroke-width", 2)
	.attr("stroke", "black");

    var track1=svg.append("line")
	.attr("x1", xScale(2))
	.attr("y1", yScale(3))
	.attr("x2", xScale(steps+1))
	.attr("y2", yScale(3))
	.attr("stroke-width", 2)
	.attr("stroke", "black");

    var colors=["blue","red"]

    //Create circles
    svg.selectAll("circle")
	.data(dataset)
	.enter()
	.append("circle")
	.attr("cy", function(d, i) {
	    return yScale(2*i+1);
	})
	.attr("cx", function(d) {
	    return xScale(d+2);
	})
	.attr("r", yScale.rangeBand())
	.attr("fill", function(d,i) {
	    return colors[i];
	});

    //On click, update with new data			
    d3.select("body")
	.on("keydown", function() {
	    var choice = d3.event.keyCode % 2
	    console.log("choice="+choice+",  scores="+dataset);
	    //New values for dataset
	    dataset[choice]=(dataset[choice]+1) % steps

	    //Update all rects
	    svg.selectAll("circle")
		.data(dataset)
		.attr("cx", function(d) {
		    return xScale(d+2);
		})
	});
})
