/* gy-header-bar.h
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

#ifndef __GY_HEADER_BAR_H__
#define __GY_HEADER_BAR_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_HEADER_BAR (gy_header_bar_get_type())

G_DECLARE_FINAL_TYPE (GyHeaderBar, gy_header_bar, GY, HEADER_BAR, GtkHeaderBar)

GyHeaderBar *gy_header_bar_new                          (void);
void         gy_header_bar_connect_entry_with_tree_view (GyHeaderBar *self,
                                                         GtkTreeView *tree_view);
void         gy_header_bar_set_text_in_entry            (GyHeaderBar *self,
                                                         const gchar *text);
GtkEntry    *gy_header_bar_get_entry                    (GyHeaderBar *self);

G_END_DECLS

#endif /* __GY_HEADER_BAR_H__ */

