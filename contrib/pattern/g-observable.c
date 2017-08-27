/* g-observable.c
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

#include "g-observable.h"
#include "g-observer.h"

G_DEFINE_INTERFACE (GObservable, g_observable, G_TYPE_OBJECT)

static void
g_observable_default_init (GObservableInterface *iface)
{
}

void
g_observable_add_observer (GObservable *observable,
                           GObserver   *observer)
{
  GObservableInterface *iface;

  g_return_if_fail (G_IS_OBSERVABLE (observable));
  g_return_if_fail (G_IS_OBSERVER (observer));

  iface = G_OBSERVABLE_GET_IFACE (observable);

  g_return_if_fail (iface->add_observer != NULL);

  iface->add_observer (observable, observer);
}

void
g_observable_delete_observer (GObservable *observable,
                              GObserver   *observer)
{
  GObservableInterface *iface;

  g_return_if_fail (G_IS_OBSERVABLE (observable));
  g_return_if_fail (G_IS_OBSERVER (observer));

  iface = G_OBSERVABLE_GET_IFACE (observable);

  g_return_if_fail (iface->delete_observer != NULL);

  iface->delete_observer (observable, observer);
}

void
g_observable_delete_all_observers (GObservable *observable)
{
  GObservableInterface *iface;

  g_return_if_fail (G_IS_OBSERVABLE (observable));

  iface = G_OBSERVABLE_GET_IFACE (observable);

  g_return_if_fail (iface->delete_all_observers != NULL);

  iface->delete_all_observers (observable);
}

void
g_observable_notify_observers (GObservable *observable)
{
  GObservableInterface *iface;

  g_return_if_fail (G_IS_OBSERVABLE (observable));

  iface = G_OBSERVABLE_GET_IFACE (observable);

  g_return_if_fail (iface->notify_observers != NULL);

  iface->notify_observers (observable);
}

gint
g_observable_count_observers (GObservable *observable)
{
  GObservableInterface *iface;

  g_return_val_if_fail (G_IS_OBSERVABLE (observable), 0);

  iface = G_OBSERVABLE_GET_IFACE (observable);

  g_return_val_if_fail (iface->count_observers != NULL, 0);

  return iface->count_observers (observable);
}
