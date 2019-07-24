/* gy-window.h
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

#ifndef __GY_WINDOW_H__
#define __GY_WINDOW_H__

#if !defined (GYDICT_INSIDE) && !defined (GYDICT_COMPILATION)
#error "Only <gydict.h> can be included directly."
#endif

#include "app/gy-app.h"
#include "dictionaries/gy-dict-manager.h"

G_BEGIN_DECLS

#define GY_TYPE_WINDOW (gy_window_get_type ())

G_DECLARE_FINAL_TYPE (GyWindow, gy_window, GY, WINDOW, DzlApplicationWindow)

GtkWidget*     gy_window_new             (GyApp *application);
GtkWidget*     gy_window_get_text_view   (GyWindow *self);
void           gy_window_grab_focus      (GyWindow *self);
void           gy_window_clear_search_entry (GyWindow *self);
DzlDockBin*    gy_window_get_dockbin (GyWindow *self);
GyDictManager* gy_window_get_dict_manager (GyWindow *self);

G_END_DECLS

#endif /* end of include guard: __GY_WINDOW_H__ */
