/* gy-store-entry.h
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

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_STORE_ENTRY (gy_store_entry_get_type())

G_DECLARE_FINAL_TYPE (GyStoreEntry, gy_store_entry, GY, STORE_ENTRY, GtkListBox)

void gy_store_entry_add_row (GyStoreEntry *self,
                             const gchar  *entry,
                             guint         idx);
void gy_store_entry_remove_all (GyStoreEntry *self);

G_END_DECLS
