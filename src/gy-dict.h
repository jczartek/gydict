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

#ifndef _GY_DICTIONARY_H_
#define _GY_DICTIONARY_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_DICT             (gy_dict_get_type ())

G_DECLARE_DERIVABLE_TYPE (GyDict, gy_dict, GY, DICT, GObject)


struct _GyDictClass
{
  GObjectClass parent_class;

  /* Deprecated */
  guint     (*set_dictionary)   (GyDict *dict);
  guint     (*init_list)        (GyDict *dict);
  /* New API */
  void      (*map)              (GyDict  *self,
                                 GError **err);
};

GType gy_dict_get_type (void) G_GNUC_CONST;
guint gy_dict_set_dictionary (GyDict *dict);
guint gy_dict_init_list (GyDict *dict);
void gy_dict_set_tree_model (GyDict *dict,
                             GtkTreeModel *model);
GtkTreeModel *gy_dict_get_tree_model  (GyDict *dict);
const gchar  *gy_dict_get_id_string (GyDict *dict);
gint gy_dict_get_encoding (GyDict *dict);
GyDict *gy_dict_new_object (const gchar *id_string);

void     gy_dict_map              (GyDict  *self,
                                   GError **err);
gboolean gy_dict_is_map           (GyDict *self);
void     gy_dict_initialize       (void);

G_END_DECLS

#endif /* _GY_DICTIONARY_H_ */

