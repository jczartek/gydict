/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * gy-print-compositor.h
 * Copyright (C) 2014 kuba <kuba@kuba>
 *
 * gy-print-compositor.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * gy-print-compositor.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GY_PRINT_COMPOSITOR_H_
#define _GY_PRINT_COMPOSITOR_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_PRINT_COMPOSITOR             (gy_print_compositor_get_type ())
#define GY_PRINT_COMPOSITOR(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GY_TYPE_PRINT_COMPOSITOR, GyPrintCompositor))
#define GY_PRINT_COMPOSITOR_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GY_TYPE_PRINT_COMPOSITOR, GyPrintCompositorClass))
#define GY_IS_PRINT_COMPOSITOR(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GY_TYPE_PRINT_COMPOSITOR))
#define GY_IS_PRINT_COMPOSITOR_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GY_TYPE_PRINT_COMPOSITOR))
#define GY_PRINT_COMPOSITOR_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GY_TYPE_PRINT_COMPOSITOR, GyPrintCompositorClass))

typedef struct _GyPrintCompositor GyPrintCompositor;
typedef struct _GyPrintCompositorClass GyPrintCompositorClass;

struct _GyPrintCompositor
{
  GObject parent_instance;
};

struct _GyPrintCompositorClass
{
  GObjectClass parent_class;
};

GType gy_print_compositor_get_type (void) G_GNUC_CONST;
/**::GETTERS::**/
gchar *gy_print_compositor_get_body_font_name    (GyPrintCompositor *compositor);
gchar *gy_print_compositor_get_header_font_name  (GyPrintCompositor *compositor);
gchar *gy_print_compositor_get_footer_font_name  (GyPrintCompositor *compositor);
gint gy_print_compositor_get_n_pages             (GyPrintCompositor *compositor);
GtkWrapMode gy_print_compositor_get_wrap_mode    (GyPrintCompositor *compositor);
/**::SETTERS::**/
void gy_print_compositor_set_wrap_mode         (GyPrintCompositor *compositor,
                                                GtkWrapMode        wrap_mode);
void gy_print_compositor_set_body_font_name    (GyPrintCompositor *compositor,
                                                const gchar       *font_name);
void gy_print_compositor_set_header_font_name  (GyPrintCompositor *compositor,
                                                const gchar       *font_name);
void gy_print_compositor_set_footer_font_name (GyPrintCompositor  *compositor,
                                               const gchar        *font_name);
/**::CREATE::**/
GyPrintCompositor * gy_print_compositor_new_from_view (GtkTextView *view);
/**::PUBLIC METHOD::**/
gboolean gy_print_compositor_paginate (GyPrintCompositor  *compositor,
                                       GtkPrintContext    *context);
void gy_print_compositor_draw_page    (GyPrintCompositor  *compositor,
                                       GtkPrintContext    *context,
                                       gint                page_nr);
G_END_DECLS

#endif /* _GY_PRINT_COMPOSITOR_H_ */

