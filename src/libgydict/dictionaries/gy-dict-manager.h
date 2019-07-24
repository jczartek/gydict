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

#if !defined (GYDICT_INSIDE) && !defined (GYDICT_COMPILATION)
#error "Only <gydict.h> can be included directly."
#endif

#include <gtk/gtk.h>
#include "gy-dict.h"

G_BEGIN_DECLS

#define GY_TYPE_DICT_MANAGER (gy_dict_manager_get_type())

G_DECLARE_FINAL_TYPE (GyDictManager, gy_dict_manager, GY, DICT_MANAGER, GObject)

GyDictManager *gy_dict_manager_new              (void);

GyDict* gy_dict_manager_lookup_dict (GyDictManager *self,
                                      const gchar   *idx);
void gy_dict_manager_insert_dict (GyDictManager *self,
                                  GyDict        *dict,
                                  const gchar   *idx);
void gy_dict_manager_remove_dict (GyDictManager *self,
                                  const gchar   *idx);



G_END_DECLS

#endif /* __GY_DICT_MANAGER_H__ */

