/* gy-window.h
 *
 * Copyright (C) 2016 Jakub Czartek <kuba@linux.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GY_WINDOW_H__
#define __GY_WINDOW_H__

#include "gy-app.h"
#include "gy-dict.h"
#include "gy-print-compositor.h"

G_BEGIN_DECLS	

/*
 * Type macros
 */

#define GY_TYPE_WINDOW            (gy_window_get_type ())
#define GY_WINDOW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GY_TYPE_WINDOW, GyWindow))
#define GY_IS_WINDOW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GY_TYPE_WINDOW))
#define GY_WINDOW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GY_TYPE_WINDOW, GyWindowClass))
#define GY_IS_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GY_TYPE_WINDOW))
#define GY_WINDOW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GY_TYPE_WINDOW, GyWindowClass))

typedef struct _GyWindow GyWindow;
typedef struct _GyWindowClass GyWindowClass;

struct _GyWindow
{
  GtkApplicationWindow parent_instance;
};

struct _GyWindowClass
{
  GtkApplicationWindowClass parent_class;
};

/* used by GY_TYPE_WINDOW */
GType gy_window_get_type (void);

/*
 * Method definitions
 */
GtkWidget *gy_window_new (GyApp *application);
GtkTextBuffer *gy_window_get_text_buffer (GyWindow *window);
GyDict *gy_window_get_dictionary (GyWindow *window);
GtkWidget *gy_window_get_header_bar (GyWindow *window);
GtkWidget *gy_window_get_entry (GyWindow *window);
GtkWidget *gy_window_get_text_view (GyWindow *window);

G_END_DECLS

#endif /* end of include guard: __GY_WINDOW_H__ */
