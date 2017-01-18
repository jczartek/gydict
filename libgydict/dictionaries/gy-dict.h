/* gy-dict.h
 *
 * Copyright (C) 2014 Jakub Czartek <kuba@linux.pl>
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

#ifndef __GY_DICT_H__
#define __GY_DICT_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_DICT             (gy_dict_get_type ())

G_DECLARE_DERIVABLE_TYPE (GyDict, gy_dict, GY, DICT, GObject)

struct _GyDictClass
{
  GObjectClass __parent__;

  void      (*map)   (GyDict  *self,
                      GError **err);
};

GtkTreeModel* gy_dict_get_tree_model  (GyDict *dict);
void          gy_dict_map             (GyDict  *self,
                                       GError **err);
gboolean      gy_dict_is_mapped       (GyDict *self);
gboolean      gy_dict_is_used         (GyDict *self);
void          gy_dict_initialize      (void);
GObject*      gy_dict_new             (const gchar   *identifier,
                                       GtkTextBuffer *buffer);

G_END_DECLS

#endif /* __GY_DICT_H__ */
