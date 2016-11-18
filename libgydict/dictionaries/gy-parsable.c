/* gy-parsable.c
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

#include "gy-parsable.h"

G_DEFINE_INTERFACE (GyParsable, gy_parsable, G_TYPE_OBJECT);

static void
gy_parsable_default_init (GyParsableInterface *klass G_GNUC_UNUSED)
{
}

void
gy_parsable_parse (GyParsable      *self,
                   GtkTextBuffer   *buffer,
                   gint             row)
{
  g_return_if_fail (GY_PARSABLE (self));

  GY_PARSABLE_GET_IFACE (self)->parse (self, buffer, row);
}
