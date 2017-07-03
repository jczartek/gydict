/* gy-tree-view.h
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

#ifndef GY_DEF_LIST_H
#define GY_DEF_LIST_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_DEF_LIST (gy_def_list_get_type())

G_DECLARE_FINAL_TYPE (GyDefList, gy_def_list, GY, DEF_LIST, GtkTreeView)

gint   gy_def_list_get_selected_n_row         (GyDefList *self);
void   gy_def_list_select_row                 (GyDefList *self,
                                               gint        row);
gchar* gy_def_list_get_value_for_selected_row (GyDefList *self);

G_END_DECLS

#endif /* GY_DEF_LIST_H */

