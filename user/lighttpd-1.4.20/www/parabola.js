function parabola(edges, axis, vertexX, vertexY, degrees, jumpBy, smoother, degreesAreInRadians)
//parabola         (4,     dt[1], dt[0],  -dt[1],      0,    .1,       1)
{
	//Validate:
	edges=(!isNaN(parseFloat(edges)))? parseFloat(edges):10;
	axis=(!isNaN(parseFloat(axis)))? parseFloat(axis):10;
	vertexX=(!isNaN(parseFloat(vertexX)))? parseFloat(vertexX):0;
	vertexY=(!isNaN(parseFloat(vertexY)))? parseFloat(vertexY):0;
	degrees=(!isNaN(parseFloat(degrees)))? parseFloat(degrees):0;
	degrees=(degreesAreInRadians)?degrees*57.296:degrees;
	while(degrees>360){degrees-=360;}
	while(degrees<0){degrees+=360;}
	degrees=degrees/57.296;/*to radians, which is required by Math.sin and Math.cos*/
	jumpBy=(!isNaN(parseFloat(jumpBy)))? Math.abs(parseFloat(jumpBy)):0.5;
	jumpBy=(jumpBy)?jumpBy:0.5;/*no zero, or runtime loop never ends!*/
	//Initialize:
	var half=Math.round( (edges/2) *100)/100;/*if vertex=(0,0), this is the unsigned x coordinate of the edges*/
	/*below:  if Vertex=(0,0), parable equation is y=ax^2 or axis=a*half^2*/
	var a=Math.round( (axis/Math.pow(half,2)) *1000000)/1000000;
	var array1=[];
	var array2=[];
	var oldY=0;
	
	//a=-a;
	/*
	Runtime:  Loop to plot the parabola: 
	applying y=ax^2, 
	then adding the vertexX and vertexY coordinates,
	then traslating via
	x' = x*cos(angle) + y*sin(angle)
	y' = -x*sin(angle) + y*cos(angle);
	this to all points.
	*/
	for(var x=0; x<=half+1; x+=jumpBy){
	var newX=x;
	var newX2=-x;
	var newY=Math.round( (a*Math.pow(x,2)) *10)/10;
	var currentY=Math.round((
	vertexY+( (-newX*Math.sin(degrees))+(newY*Math.cos(degrees)) )
	)*10)/10/*I calculate it before all if a fixed speed is required*/
	
	//alert("vertexY "+ vertexY + " newY " + newY + " a " + a + " currentY " + currentY);
	//return(array1);
	var space=Math.abs(oldY-currentY);
		if(smoother){
			if(space<jumpBy){continue;};
		oldY=currentY;
		};
	//parabola first half:
	array1[++array1.length-1]=[
	Math.round((
	vertexX+( (newX*Math.cos(degrees))+(newY*Math.sin(degrees)) )
	)*10)/10,
	currentY
	];
	//parabola second half:
	array2[++array2.length-1]=[
	Math.round((
	vertexX+( (newX2*Math.cos(degrees))+(newY*Math.sin(degrees)) )
	)*10)/10
	,
	Math.round((
	vertexY+( (-newX2*Math.sin(degrees))+(newY*Math.cos(degrees)) )
	)*10)/10
	];
	}
	//streamline:
	if(edges/jumpBy<7 || !array1.length){/*7 is a bit arbitrary a number: but a ratio lower than that would produce no clear animation.
	This accounts also if it was passed a jumpBy argument that exceeded the output array entries, thus producing an empty array*/
	window["vertexIndex"]=window["vertexindex"]=0;
	return [new Array(vertexX,vertexY, "VERTEX COORDINATES")];
	};
	array2=array2.reverse();
	array2.pop();
	
	array1[0][++array1[0].length-1]="VERTEX COORDINATES";
	window["vertexIndex"]=window["vertexindex"]=array2.length-1;
	return array2.concat(array1);
	/* keep this comment to reuse freely:
	http://www.fullposter.com/?1 */
}


	// param 1 x axis size (length of parabola)
	// param 2 hieght of the y axis with respect to forth parameter in downward direction
	// param 3 centre point on x axis
	// param 4 hieght of the y axis

	//
	//var parabolaPlot=parabola(4, -30, 0, 30, 0, 0, 1);//a possible parabola

