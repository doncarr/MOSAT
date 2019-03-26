function transform_mouse_xy(evt, svg_obj)
{
  var SVGRoot = document.getElementsByTagName("svg")[0];
  var point = SVGRoot.createSVGPoint();
  //var point = svgRoot.createSVGPoint();
  //var point = document.documentElement.createSVGPoint();
  //var point = svg_obj.createSVGPoint();
  point.x = evt.clientX;
  point.y = evt.clientY;
  var ctm = svg_obj.getScreenCTM();
  point = point.matrixTransform(ctm.inverse());
  return point;
}
