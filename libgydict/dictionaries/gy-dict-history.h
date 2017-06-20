/* gy-dict-history.h
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

#ifndef __GY_DICT_HISTORY_H__
#define __GY_DICT_HISTORY_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define GY_TYPE_DICT_HISTORY (gy_dict_history_get_type())

G_DECLARE_FINAL_TYPE (GyDictHistory, gy_dict_history, GY, DICT_HISTORY, GObject)

GyDictHistory* gy_dict_history_new       (void);
void           gy_dict_history_append    (GyDictHistory *self,
                                          gint           row_number);
void           gy_dict_history_set_state (GyDictHistory *self);
gint           gy_dict_history_go_back   (GyDictHistory *self);
gint           gy_dict_history_go_next   (GyDictHistory *self);
guint          gy_dict_history_size      (GyDictHistory *self);
void           gy_dict_history_reset_state (GyDictHistory *self);
G_END_DECLS

#endif /* GY_DICT_HISTORY_H */

