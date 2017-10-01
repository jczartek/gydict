/* gy-entry-collector.c
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

#include "gy-entry-collector.h"

G_DEFINE_INTERFACE (GyEntryCollector, gy_entry_collector, G_TYPE_OBJECT)

static void
gy_entry_collector_default_init (GyEntryCollectorInterface *iface)
{
}

gboolean
gy_entry_collector_add (GyEntryCollector *self,
                        const gchar      *entry,
                        guint             idx)
{
  GyEntryCollectorInterface *iface;

  g_return_val_if_fail (GY_IS_ENTRY_COLLECTOR (self), FALSE);
  g_return_val_if_fail (entry != NULL, FALSE);
  g_return_val_if_fail (idx < G_MAXUINT, FALSE);

  iface = GY_ENTRY_COLLECTOR_GET_IFACE (self);
  g_return_val_if_fail (iface->add != NULL, FALSE);

  return iface->add(self, entry, idx);
}

void
gy_entry_collector_remove (GyEntryCollector *self,
                           guint             idx)
{
  GyEntryCollectorInterface *iface;

  g_return_if_fail (GY_IS_ENTRY_COLLECTOR (self));
  g_return_if_fail (idx < G_MAXUINT);

  iface = GY_ENTRY_COLLECTOR_GET_IFACE (self);
  g_return_if_fail (iface->remove != NULL);

  iface->remove (self, idx);
}

void
gy_entry_collector_foreach (GyEntryCollector *self,
                            GTraverseFunc     func,
                            gpointer          data)
{
  GyEntryCollectorInterface *iface;

  g_return_if_fail (GY_IS_ENTRY_COLLECTOR (self));
  g_return_if_fail (func != NULL);

  iface = GY_ENTRY_COLLECTOR_GET_IFACE (self);
  g_return_if_fail (iface->foreach != NULL);

  iface->foreach (self, func, data);
}

