/* gy-entry-collector.h
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

#define GY_TYPE_ENTRY_COLLECTOR (gy_entry_collector_get_type ())

G_DECLARE_INTERFACE (GyEntryCollector, gy_entry_collector, GY, ENTRY_COLLECTOR, GObject)

struct _GyEntryCollectorInterface
{
  GTypeInterface parent;

  gboolean (*add)     (GyEntryCollector *self,
                       const gchar      *entry,
                       guint             idx);

  void     (*remove)  (GyEntryCollector *self,
                       guint             idx);

  void     (*foreach) (GyEntryCollector *self,
                       GTraverseFunc     func,
                       gpointer          data);
};

gboolean gy_entry_collector_add     (GyEntryCollector *self,
                                     const gchar      *entry,
                                     guint             idx);
void gy_entry_collector_remove      (GyEntryCollector *self,
                                     guint             idx);
void gy_entry_collector_foreach     (GyEntryCollector *self,
                                     GTraverseFunc     func,
                                     gpointer          data);

G_END_DECLS
