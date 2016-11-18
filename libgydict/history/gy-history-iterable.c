/*
 * gy-history-iterable.c
 * Copyright (C) 2015 kuba <kuba@linux.pl>
 *
 * gy-history-iterable.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * gy-history-iterable.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "gy-history-iterable.h"

G_DEFINE_INTERFACE (GyHistoryIterable, gy_history_iterable, 0);

static void
gy_history_iterable_default_init (GyHistoryIterableInterface *iface)
{
}

/**
 * gy_history_iterable_next_item:
 * @iterable: a GyHistoryIterable
 *
 * Moves an iterator to the next position after @iterable.
 */
void
gy_history_iterable_next_item (GyHistoryIterable *iterable)
{
  g_return_if_fail (GY_HISTORY_IS_ITERABLE (iterable));

  GY_HISTORY_ITERABLE_GET_INTERFACE (iterable)->next (iterable);
}


/**
 * gy_history_iterable_previous_item:
 * @iterable: a GyHistoryIterable
 *
 * Moves an iterator to the previous position before @iterable.
 */
void
gy_history_iterable_previous_item (GyHistoryIterable *iterable)
{
  g_return_if_fail (GY_HISTORY_IS_ITERABLE (iterable));

  GY_HISTORY_ITERABLE_GET_INTERFACE (iterable)->previous (iterable);
}

/**
 * gy_history_iterable_is_begin:
 * @iterable: a GyHistoryIterable
 *
 * Checks whether @iterable is the begin iterator.
 *
 * Returns: TRUE when @iterable is the begin iterator, otherwise FALSE.
 */
gboolean
gy_history_iterable_is_begin (GyHistoryIterable *iterable)
{
  g_return_val_if_fail (GY_HISTORY_IS_ITERABLE (iterable), TRUE);

  return GY_HISTORY_ITERABLE_GET_INTERFACE (iterable)->is_begin (iterable);
}

/**
 * gy_history_iterable_is_end:
 * @iter: a GyHistoryIterable
 *
 * Checks whether @iterable is the end iterator.
 *
 * Returns: TRUE when @iterable is the end iterator, otherwise FALSE.
 */
gboolean
gy_history_iterable_is_end (GyHistoryIterable *iterable)
{
  g_return_val_if_fail (GY_HISTORY_IS_ITERABLE (iterable), TRUE);

  return GY_HISTORY_ITERABLE_GET_INTERFACE (iterable)->is_end (iterable);
}

/**
 * gy_history_iterable_get_item:
 * @iter: a GyHistoryIterable
 *
 * Returns the data that @iterable points to.
 *
 * Returns: the data that @iterable points to.
 */
gconstpointer
gy_history_iterable_get_item (GyHistoryIterable *iterable)
{
  g_return_val_if_fail (GY_HISTORY_IS_ITERABLE (iterable), NULL);

  return GY_HISTORY_ITERABLE_GET_INTERFACE (iterable)->get (iterable);
}

