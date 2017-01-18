/* gy-dict-manager.h
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
#ifndef __GY_DICT_MANAGER_H__
#define __GY_DICT_MANAGER_H__

#include <gtk/gtk.h>
#include "gy-dict.h"

G_BEGIN_DECLS

#define GY_TYPE_DICT_MANAGER (gy_dict_manager_get_type())

G_DECLARE_FINAL_TYPE (GyDictManager, gy_dict_manager, GY, DICT_MANAGER, GObject)

GyDictManager *gy_dict_manager_new           (void);
GyDict        *gy_dict_manager_add_dict      (GyDictManager *self,
                                              const gchar   *name_dict,
                                              GtkTextBuffer *buffer);
GyDict        *gy_dict_manager_get_used_dict (GyDictManager *self);

G_END_DECLS

#endif /* __GY_DICT_MANAGER_H__ */

