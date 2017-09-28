/* gy-window-settings.h
 *
 * Copyright (C) 2017 Jakub Czartek <kuba@linux.pl>
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

#ifndef GY_WINDOW_SETTINGS_H
#define GY_WINDOW_SETTINGS_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

void gy_window_settings_register (GtkWindow *window);

G_END_DECLS

#endif /* GY_WINDOW_SETTINGS_H */
