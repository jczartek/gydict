/* g-pattern-types.h
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

#ifndef G_PATTERN_TYPES_H
#define G_PATTERN_TYPES_H

#include <glib-object.h>

G_BEGIN_DECLS

#define G_TYPE_OBSERVABLE (g_observable_get_type ())

G_DECLARE_INTERFACE (GObservable, g_observable, G, OBSERVABLE, GObject)

#define G_TYPE_OBSERVER (g_observer_get_type ())

G_DECLARE_INTERFACE (GObserver, g_observer, G, OBSERVER, GObject)
G_END_DECLS

#endif /* G_PATTERN_TYPES_H */
