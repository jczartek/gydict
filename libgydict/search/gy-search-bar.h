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

#ifndef GY_SEARCH_BAR_H
#define GY_SEARCH_BAR_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_SEARCH_BAR (gy_search_bar_get_type())

G_DECLARE_FINAL_TYPE (GySearchBar, gy_search_bar, GY, SEARCH_BAR, GtkBin)

GySearchBar *gy_search_bar_new (void);

G_END_DECLS

#endif /* GY_SEARCH_BAR_H */

