/* gy-utility-func.h
 *
 * Copyright (C) 2014 Jakub Czartek <kuba@linux.pl>
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

#ifndef __GY_UTILITY_H__
#define __GY_UTILITY_H__

#if !defined (GYDICT_INSIDE) && !defined (GYDICT_COMPILATION)
#error "Only <gydict.h> can be included directly."
#endif

#include <gtk/gtk.h>
#include <pango/pango.h>

G_BEGIN_DECLS

extern gboolean gy_utility_handlers_is_blocked_by_func(gpointer instance,
                                                       gpointer func,
                                                       gpointer data);
gboolean gy_utility_is_handler_connected (gpointer instance,
                                          gpointer handler);
extern gint gy_utility_strcmp (const gchar *p1,
                               const gchar *p2,
                               size_t n);
extern gchar *gy_utility_compute_md5_for_file (GFile  *file,
                                               GError **err);

G_END_DECLS

#endif
