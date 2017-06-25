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

#ifndef GY_TREE_VIEW_H
#define GY_TREE_VIEW_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_TREE_VIEW (gy_tree_view_get_type())

G_DECLARE_FINAL_TYPE (GyTreeView, gy_tree_view, GY, TREE_VIEW, GtkTreeView)

gint gy_tree_view_get_selected_row_number (GyTreeView *self);
void gy_tree_view_select_row              (GyTreeView *self,
                                           gint        row);
gchar *gy_tree_view_get_value_for_selected_row (GyTreeView *self);

G_END_DECLS

#endif /* GY_TREE_VIEW_H */

