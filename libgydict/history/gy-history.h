/* gy-history.h
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

#ifndef __GY_HISTORY_H__
#define __GY_HISTORY_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define GY_TYPE_HISTORY (gy_history_get_type ())

G_DECLARE_FINAL_TYPE (GyHistory, gy_history, GY, HISTORY, GObject)

/*
 * Method definitions
 */
GyHistory* gy_history_new       (void);
void       gy_history_append    (GyHistory   *obj,
                                 const gchar *str);
void       gy_history_update    (GyHistory   *obj);
guint      gy_history_length    (GyHistory   *obj);

G_END_DECLS

#endif /* end of include guard: __GY_HISTORY_H__ */
