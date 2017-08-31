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

G_BEGIN_DECLS

#define G_TYPE_OBSERVABLE (g_observable_get_type())

G_DECLARE_FINAL_TYPE (GObservable, g_observable, G, OBSERVABLE, GObject)

GObservable* g_observable_new                  (GObject *owner);

void         g_observable_add_observer         (GObservable *self,
                                                GObserver   *observer);
void         g_observable_delete_observer      (GObservable *self,
                                                GObserver   *observer);
void         g_observable_delete_all_observers (GObservable *self);
void         g_observable_notify_observers     (GObservable  *self,
                                                const GValue *arg);
gint         g_observable_count_observers      (GObservable *self);
void         g_observable_dispatch_boolean     (GObservable *self,
                                                gboolean     arg);
void         g_observable_dispatch_char        (GObservable *self,
                                                gchar        arg);
void         g_observable_dispatch_int         (GObservable *self,
                                                gint         arg);
void         g_observable_dispatch_long        (GObservable *self,
                                                long         arg);
void         g_observable_dispatch_float       (GObservable *self,
                                                float        arg);
void         g_observable_dispatch_double      (GObservable *self,
                                                double       arg);
void         g_observable_dispatch_string      (GObservable *self,
                                                const gchar *arg);
void         g_observable_dispatch_pointer     (GObservable *self,
                                                gpointer     arg);
void         g_observable_dispatch_object      (GObservable *self,
                                                GObject     *arg);
G_END_DECLS

#endif /* G_OBSERVABLE_H */
