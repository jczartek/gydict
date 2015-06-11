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
  g_object_interface_install_property (iface,
				       g_param_spec_boolean ("is-begin",
				                             "IsBegin",
				                             "This property indicate whether a iterator is the begin iterator",
				                             FALSE,
				                             G_PARAM_READWRITE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));

  g_object_interface_install_property (iface,
				       g_param_spec_boolean ("is-end",
				                             "IsEnd",
				                             "This property indicate whether a iterator is the end iterator",
				                             FALSE,
				                             G_PARAM_READWRITE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB));
}

/**
 * gy_history_iterable_next_item:
 * @iter: a GyHistoryIterable
 *
 * Moves an iterator to the next position after @iter.
 */
void
gy_history_iterable_next_item (GyHistoryIterable *iter)
{
  g_return_if_fail (GY_HISTORY_IS_ITERABLE (iter));

  GY_HISTORY_ITERABLE_GET_INTERFACE (iter)->next (iter);
}


/**
 * gy_history_iterable_previous_item:
 * @iter: a GyHistoryIterable
 *
 * Moves an iterator to the previous position before @iter.
 */
void
gy_history_iterable_previous_item (GyHistoryIterable *iter)
{
  g_return_if_fail (GY_HISTORY_IS_ITERABLE (iter));

  GY_HISTORY_ITERABLE_GET_INTERFACE (iter)->previous (iter);
}

/**
 * gy_history_iterable_is_begin:
 * @iter: a GyHistoryIterable
 *
 * Checks whether @iter is the begin iterator.
 *
 * Returns: TRUE when @iter is the begin iterator, otherwise FALSE.
 */
gboolean
gy_history_iterable_is_begin (GyHistoryIterable *iter)
{
  g_return_val_if_fail (GY_HISTORY_IS_ITERABLE (iter), TRUE);

  return GY_HISTORY_ITERABLE_GET_INTERFACE (iter)->is_begin (iter);
}

/**
 * gy_history_iterable_is_end:
 * @iter: a GyHistoryIterable
 *
 * Checks whether @iter is the end iterator.
 *
 * Returns: TRUE when @iter is the end iterator, otherwise FALSE.
 */
gboolean
gy_history_iterable_is_end (GyHistoryIterable *iter)
{
  g_return_val_if_fail (GY_HISTORY_IS_ITERABLE (iter), TRUE);

  return GY_HISTORY_ITERABLE_GET_INTERFACE (iter)->is_end (iter);
}

/**
 * gy_history_iterable_get_item:
 * @iter: a GyHistoryIterable
 *
 * Returns the data that @iter points to.
 *
 * Returns: the data that @iter points to.
 */
gconstpointer
gy_history_iterable_get_item (GyHistoryIterable *iter)
{
  g_return_val_if_fail (GY_HISTORY_IS_ITERABLE (iter), NULL);

  return GY_HISTORY_ITERABLE_GET_INTERFACE (iter)->get (iter);
}

