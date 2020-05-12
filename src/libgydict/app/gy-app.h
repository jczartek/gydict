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

#if !defined (GYDICT_INSIDE) && !defined (GYDICT_COMPILATION)
#error "Only <gydict.h> can be included directly."
#endif

#include <gtk/gtk.h>
#include <dazzle.h>

#include "../services/gy-service-provider.h"

G_BEGIN_DECLS

#define GY_TYPE_APP    (gy_app_get_type ())
#define GY_APP_DEFAULT (GY_APP (g_application_get_default ()))

G_DECLARE_FINAL_TYPE (GyApp, gy_app, GY, APP, DzlApplication)

GyApp *gy_app_new            (void);
void   gy_app_new_window     (GyApp *self);
GyServiceProvider* gy_app_get_service_provider (GyApp *self);
G_END_DECLS

#endif /* end of include guard: __GY_APP_H__ */
