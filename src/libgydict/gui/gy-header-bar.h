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

#if !defined (GYDICT_INSIDE) && !defined (GYDICT_COMPILATION)
#error "Only <gydict.h> can be included directly."
#endif

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_HEADER_BAR (gy_header_bar_get_type())

G_DECLARE_FINAL_TYPE (GyHeaderBar, gy_header_bar, GY, HEADER_BAR, GtkHeaderBar)

GyHeaderBar *gy_header_bar_new                          (void);

void         gy_header_bar_add_primary (GyHeaderBar *self,
                                        GtkWidget   *widget);

void         gy_header_bar_add_center_left (GyHeaderBar *self,
                                            GtkWidget   *child);

void         gy_header_bar_add_secondary (GyHeaderBar *self,
                                          GtkWidget   *widget);

G_END_DECLS

#endif /* __GY_HEADER_BAR_H__ */

