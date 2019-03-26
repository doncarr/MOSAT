function slider_timeout_popup()
{
  slider_object_popup.timeout();
}
function slider_mousedown_popup(evt)
{
  slider_object_popup.mousedown(evt);
}
function slider_mouseup_popup(evt)
{
  slider_object_popup.mouseup(evt);
}
function slider_mousemove_popup(evt)
{
  slider_object_popup.mousemove(evt);
}
document.addEventListener("mouseup", slider_mouseup_popup, false);
document.addEventListener("mousemove", slider_mousemove_popup, false);
var slider_object_popup = new slider_t("PVALVE", "slider_circle_popup", "slider_rect_popup", "slider_text_popup", 7.5, 20, 105, 3.15, "slider_timeout_popup()");
