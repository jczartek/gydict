/* gy-app.h
 *
 * Copyright (C) 2015 Jakub Czartek <kuba@linux.pl>
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

#ifndef __GY_APP_H__
#define __GY_APP_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_APP (gy_app_get_type ())

G_DECLARE_FINAL_TYPE (GyApp, gy_app, GY, APP, GtkApplication)

GyApp *gy_app_new            (void);
void   gy_app_new_window     (GyApp *self);
GMenu *gy_app_get_menu_by_id (GyApp       *self,
                              const gchar *id);
G_END_DECLS

#endif /* end of include guard: __GY_APP_H__ */
