#ifndef ACTRCANVAS_H
#define ACTRCANVAS_H

extern void actr_canvas2d_fill_style(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
extern void actr_canvas2d_stroke_style(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);

extern void actr_canvas2d_begin_path();

extern void actr_canvas2d_arc(float x, float y, float radius, float startAngle, float endAngle, int counterclockwise);
extern void actr_canvas2d_arcTo(float x1, float y1, float x2, float y2, float radius);

extern void actr_canvas2d_ellipse(float x, float y, float radiusX, float radiusY, float rotation, float startAngle, float endAngle, char counterclockwise);

extern void actr_canvas2d_fill();
extern void actr_canvas2d_fill_rect(float x, float y, float w, float h);
extern void _actr_canvas2d_fill_text_length(float x, float y, const char *text, int length);

extern unsigned int actr_canvas2d_pick(int x, int y);
extern void actr_canvas2d_fill_gradient_all(int x, int y, int w, int h);
extern void actr_canvas2d_fill_gradient_pick(int x, int y, int w , int h, unsigned char r, unsigned char g, unsigned char b);
    

extern void _actr_canvas2d_measure_text_length(char * text, int length);
void actr_canvas2d_measure_text(char * text) {
    _actr_canvas2d_measure_text_length(text, strlen(text));
}

extern void actr_canvas2d_lineto(float x, float y);
extern void actr_canvas2d_moveto(float x, float y);
extern void actr_canvas2d_close_path();

extern void actr_canvas2d_stroke();
extern void actr_canvas2d_stroke_rect(float x, float y, float w, float h);

void actr_canvas2d_fill_text(float x, float y, const char *text) {
    _actr_canvas2d_fill_text_length(x, y, text, strlen(text));
}

#endif