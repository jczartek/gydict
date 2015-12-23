/*
 * gy-history-iterable.h
 * Copyright (C) 2015 kuba <kuba@linux.pl>
 *
 * gy-history-iterable.h is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * gy-history-iterable.h is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GY_HISTORY_ITERABLE_H__
#define __GY_HISTORY_ITERABLE_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_HISTORY_TYPE_ITERABLE                (gy_history_iterable_get_type ())
#define GY_HISTORY_ITERABLE(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), GY_HISTORY_TYPE_ITERABLE, GyHistoryIterable))
#define GY_HISTORY_IS_ITERABLE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GY_HISTORY_TYPE_ITERABLE))
#define GY_HISTORY_ITERABLE_GET_INTERFACE(inst)	(G_TYPE_INSTANCE_GET_INTERFACE ((inst), GY_HISTORY_TYPE_ITERABLE, GyHistoryIterableInterface))

typedef struct _GyHistoryIterable          GyHistoryIterable; /* dumny object */
typedef struct _GyHistoryIterableInterface GyHistoryIterableInterface;

typedef GList GyHistoryIter;

struct _GyHistoryIterableInterface
{
  GTypeInterface parent_iface;

  void          (*next)      (GyHistoryIterable *iterable);
  void          (*previous)  (GyHistoryIterable *iterable);
  gboolean      (*is_begin)  (GyHistoryIterable *iterable);
  gboolean      (*is_end)    (GyHistoryIterable *iterable);
  gconstpointer (*get)       (GyHistoryIterable *iterable);
};

GType           gy_history_iterable_get_type        (void);
void            gy_history_iterable_next_item       (GyHistoryIterable *iterable);
void            gy_history_iterable_previous_item   (GyHistoryIterable *iterable);
gboolean        gy_history_iterable_is_begin        (GyHistoryIterable *iterable);
gboolean        gy_history_iterable_is_end          (GyHistoryIterable *iterable);
gconstpointer   gy_history_iterable_get_item        (GyHistoryIterable *iterable);

G_END_DECLS

#endif /* __GY_HISTORY_ITERABLE_H__ */
