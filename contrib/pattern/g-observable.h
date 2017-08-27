/* g-observable.h
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

#ifndef G_OBSERVABLE_H
#define G_OBSERVABLE_H

#include <glib-object.h>
#include "g-pattern-types.h"

G_BEGIN_DECLS

struct _GObservableInterface
{
  GTypeInterface parent;

  void (*add_observer)         (GObservable *observable,
                                GObserver   *observer);

  void (*delete_observer)      (GObservable *observable,
                                GObserver   *observer);

  void (*delete_all_observers) (GObservable *observable);

  void (*notify_observers)     (GObservable *observable);

  gint (*count_observers)      (GObservable *observable);

};

void g_observable_add_observer         (GObservable *observable,
                                        GObserver   *observer);
void g_observable_delete_observer      (GObservable *observable,
                                        GObserver   *observer);
void g_observable_delete_all_observers (GObservable *observable);
void g_observable_notify_observers     (GObservable *observable);
gint g_observable_count_observers      (GObservable *observable);

G_END_DECLS

#endif /* G_OBSERVABLE_H */
