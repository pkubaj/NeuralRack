/*
 * ViewPort.c
 *
 * SPDX-License-Identifier:  BSD-3-Clause
 *
 * Copyright (C) 2024 brummer <brummer@web.de>
 */

#include "xwidgets.h"

/*---------------------------------------------------------------------
-----------------------------------------------------------------------    
                
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

static void set_viewpoint(void *w_, void* ) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = (Widget_t*)w->parent;
    Widget_t *slider = p->childlist->childs[1];
    adj_set_state(slider->adj, adj_get_state(w->adj));
    int v = (int)max(0,adj_get_value(w->adj));
    slider = p->childlist->childs[2];
    adj_set_state(slider->adj, 1.0- adj_get_state(w->adj_x));
    int vx = (int)max(0,adj_get_value(w->adj_x));
    XMoveWindow(w->app->dpy,w->widget,-10*vx, -10*v);
    
}

static void adjust_viewport(void *w_, void* ) {
    Widget_t *parent = (Widget_t*)w_;
    Widget_t *w = parent->childlist->childs[0];
    XWindowAttributes attrs;
    XGetWindowAttributes(parent->app->dpy, (Window)parent->widget, &attrs);
    int height_t = attrs.height;
    int width_t = attrs.width;
    XGetWindowAttributes(parent->app->dpy, (Window)w->widget, &attrs);
    int height = attrs.height;
    int width = attrs.width;
    float d = (float)height/(float)height_t;
    float max_value = (float)((float)height/((float)(height_t/(float)(height-height_t))*(d*10.0)));
    float value = adj_get_value(w->adj);
    w->adj_y->max_value = max_value;
    if (max_value <= value) adj_set_value(w->adj,value);

    d = (float)width/(float)width_t;
    max_value = (float)((float)width/((float)(width_t/(float)(width-width_t))*(d*10.0)));
    w->adj_x->max_value = max_value;
    value = adj_get_value(w->adj_x);
    if (max_value <= value) adj_set_value(w->adj_x,value);
    
    set_viewpoint(w, NULL);
}

static void draw_viewport(void *w_, void* ) {
    Widget_t *w = (Widget_t*)w_;
    cairo_set_source_rgba(w->crb,  0.13, 0.13, 0.13, 1.0);
    cairo_paint (w->crb);
    
}

static void draw_viewslider(void *w_, void* ) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = (Widget_t*)w->parent;
    Widget_t *viewport = p->childlist->childs[0];
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    int width = attrs.width;
    int height = attrs.height;
    XWindowAttributes vp_attr;
    XGetWindowAttributes(w->app->dpy, (Window)p->widget, &vp_attr);
    int visible_h = vp_attr.height;
    XGetWindowAttributes(w->app->dpy, (Window)viewport->widget, &vp_attr);
    int content_h = vp_attr.height;
    use_bg_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, 0, 0, width, height);
    cairo_fill(w->crb);
    if (content_h <= visible_h) return;
    float state = adj_get_state(w->adj);
    float ratio = (float)visible_h / (float)content_h;
    int handle_h = (int)(ratio * height);
    if (handle_h < 10) handle_h = 10;
    if (handle_h > height) handle_h = height;
    int travel = height - handle_h;
    int pos = (int)(state * travel);
    use_base_color_scheme(w, PRELIGHT_);
    cairo_set_line_cap(w->crb, CAIRO_LINE_CAP_ROUND); 
    cairo_set_line_join(w->crb, CAIRO_LINE_JOIN_ROUND);
    cairo_move_to(w->crb, 5, pos+5);
    cairo_line_to(w->crb,5, pos+handle_h-5);
    cairo_set_line_width(w->crb,8);
   // cairo_rectangle(w->crb, 0, pos, width, handle_h);
    cairo_stroke(w->crb);
}

static void draw_viewsliderx(void *w_, void* ) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = (Widget_t*)w->parent;
    Widget_t *viewport = p->childlist->childs[0];
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    int width = attrs.width;
    int height = attrs.height;
    XWindowAttributes vp_attr;
    XGetWindowAttributes(w->app->dpy, (Window)p->widget, &vp_attr);
    int visible_w = vp_attr.width;
    XGetWindowAttributes(w->app->dpy, (Window)viewport->widget, &vp_attr);
    int content_w = vp_attr.width;
    use_bg_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, 0, 0, width, height);
    cairo_fill(w->crb);
    if (content_w <= visible_w) return;
    float state = 1.0 - adj_get_state(w->adj_x);
    float ratio = (float)visible_w / (float)content_w;
    int handle_w = (int)(ratio * width);
    if (handle_w < 10) handle_w = 10;
    if (handle_w > width) handle_w = width;
    int travel = width - handle_w;
    int pos = (int)(state * travel);
    use_base_color_scheme(w, PRELIGHT_);
    cairo_set_line_cap(w->crb, CAIRO_LINE_CAP_ROUND); 
    cairo_set_line_join(w->crb, CAIRO_LINE_JOIN_ROUND);
    cairo_move_to(w->crb,pos+5 , 5);
    cairo_line_to(w->crb,pos+handle_w-5, 5);
    cairo_set_line_width(w->crb,8);
    //cairo_rectangle(w->crb, pos, 0, handle_w, height);
    cairo_stroke(w->crb);
}

static void set_viewport(void *w_, void* ) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = (Widget_t*)w->parent;
    Widget_t *viewport = p->childlist->childs[0];
    adj_set_state(viewport->adj, adj_get_state(w->adj));
}

static void set_viewportx(void *w_, void* ) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = (Widget_t*)w->parent;
    Widget_t *viewport = p->childlist->childs[0];
    adj_set_state(viewport->adj_x, 1.0- adj_get_state(w->adj_x));
}

Widget_t* add_viewport(Widget_t *parent,int x, int y,  int width, int height) {
    Widget_t *w = create_widget(parent->app, parent, x, y, width, height);
    w->scale.gravity = CENTER; //NORTHWEST;
    w->func.expose_callback = draw_viewport;

    Widget_t *wid = create_widget(parent->app, w, 0, 0, width+10, height+10);
    XSelectInput(wid->app->dpy, wid->widget,StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask|KeyReleaseMask
                    |ButtonPressMask|Button1MotionMask|PointerMotionMask);
    wid->scale.gravity = NONE;
    wid->parent = w;
    wid->flags &= ~USE_TRANSPARENCY;
    wid->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    XWindowAttributes attrs;
    XGetWindowAttributes(parent->app->dpy, (Window)w->widget, &attrs);
    int height_t = attrs.height;
    int width_t = attrs.width;
    float d = (float)height/(float)height_t;
    float max_value = (float)((float)height/((float)(height_t/(float)(height-height_t))*(d*10.0)));
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0,max_value ,3.0, CL_VIEWPORT);
    wid->adj = wid->adj_y;
    wid->func.adj_callback = set_viewpoint;
    adj_set_value(wid->adj,0.0);

    d = (float)width/(float)width_t;
    max_value = (float)((float)width/((float)(width_t/(float)(width-width_t))*(d*10.0)));
    wid->adj_x = add_adjustment(wid,0.0, 0.0, 0.0,max_value ,3.0, CL_VIEWPORT);
    adj_set_value(wid->adj_x,0.0);

    wid->func.expose_callback = draw_viewport;

    Widget_t *slider = add_vslider(w, "", width-10, 0, 10, height-10);
    slider->func.expose_callback = draw_viewslider;
    slider->adj_y = add_adjustment(slider,0.0, 0.0, 0.0, 1.0,0.0085, CL_VIEWPORTSLIDER);
    slider->adj = slider->adj_y;
    slider->parent = w;
    slider->func.value_changed_callback = set_viewport;
    slider->scale.gravity = WESTSOUTH;
    slider->flags &= ~USE_TRANSPARENCY;
    slider->flags |= NO_AUTOREPEAT | NO_PROPAGATE;

    slider = add_hslider(w, "", 0, height-10, width-10, 10);
    slider->func.expose_callback = draw_viewsliderx;
    slider->adj_x = add_adjustment(slider,0.0, 0.0, 0.0, 1.0,0.0085, CL_VIEWPORTSLIDER);
    slider->adj = slider->adj_x;
    slider->parent = w;
    slider->func.value_changed_callback = set_viewportx;
    slider->scale.gravity = EASTSOUTH;
    slider->flags &= ~USE_TRANSPARENCY;
    slider->flags |= NO_AUTOREPEAT | NO_PROPAGATE;

    w->func.configure_notify_callback = adjust_viewport;

    return wid;
}
