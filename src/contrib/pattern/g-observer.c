/* g-observer.c
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

#include "g-observer.h"

G_DEFINE_INTERFACE (GObserver, g_observer, G_TYPE_OBJECT)

static void
g_observer_default_init (GObserverInterface *iface)
{
}

void
g_observer_update (GObserver    *observer,
                   GObject      *observable,
                   const GValue *arg)
{
  GObserverInterface *iface;

  g_return_if_fail (G_IS_OBSERVER (observer));

  iface = G_OBSERVER_GET_IFACE (observer);

  g_return_if_fail (iface->update != NULL);

  iface->update (observer, observable, arg);
}