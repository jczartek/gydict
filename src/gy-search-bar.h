/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * gy-search-bar.h
 * Copyright (C) 2014 kuba <kuba@linux.pl>
 *
 * gy-search-bar.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * gy-search-bar.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GY_SEARCH_BAR_H_
#define _GY_SEARCH_BAR_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_SEARCH_BAR             (gy_search_bar_get_type ())
#define GY_SEARCH_BAR(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GY_TYPE_SEARCH_BAR, GySearchBar))
#define GY_SEARCH_BAR_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GY_TYPE_SEARCH_BAR, GySearchBarClass))
#define GY_IS_SEARCH_BAR(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GY_TYPE_SEARCH_BAR))
#define GY_IS_SEARCH_BAR_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GY_TYPE_SEARCH_BAR))
#define GY_SEARCH_BAR_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GY_TYPE_SEARCH_BAR, GySearchBarClass))


typedef struct _GySearchBar GySearchBar;
typedef struct _GySearchBarClass GySearchBarClass;

struct _GySearchBar
{
	GtkBin parent_instance;
};

struct _GySearchBarClass
{
	GtkBinClass parent_class;
};

GType gy_search_bar_get_type (void) G_GNUC_CONST;
GtkWidget * gy_search_bar_new (void);
void gy_search_bar_set_search_mode (GySearchBar *bar, 
				    gboolean         search_mode);
void gy_search_bar_connect_text_buffer (GySearchBar *bar,
					GtkTextBuffer   *buffer);

G_END_DECLS

#endif /* _GY_SEARCH_BAR_H_ */

