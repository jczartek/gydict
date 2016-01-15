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

enum
{
  GY_OK,
  GY_MEMORY_ERROR,
  GY_READ_FILE_ERROR,
  GY_OPEN_FILE_ERROR,
  GY_SEEK_FILE_ERROR,
  GY_EXISTS_FILE_ERROR,
  GY_INVALID_ID_ERROR,
  GY_FAILED_OBJECT,
  GY_UNIMPLEMENTED_METHOD,
  GY_LAST_ERROR
};

struct _GyDictClass
{
  GObjectClass parent_class;

  /* Deprecated */
  guint     (*set_dictionary)   (GyDict *dict);
  guint     (*init_list)        (GyDict *dict);
  gpointer  (*read_definition)  (GyDict *dict,
                                 guint   index);
  /* New API */
  void      (*map)              (GyDict  *self,
                                 GError **err);
  gchar *   (*get_lexical_unit) (GyDict  *self,
                                 guint    index);

    /* Signals */
  void(* __error)               (GyDict      *self,
                                 const gchar *name_error,
                                 gpointer     data);
};

GType gy_dict_get_type (void) G_GNUC_CONST;
guint gy_dict_set_dictionary (GyDict *dict);
guint gy_dict_init_list (GyDict *dict);
gpointer gy_dict_read_definition (GyDict *dict,
                                  guint index);
void gy_dict_set_tree_model (GyDict *dict,
                             GtkTreeModel *model);
GtkTreeModel *gy_dict_get_tree_model  (GyDict *dict);
const gchar  *gy_dict_get_id_string (GyDict *dict);
gint gy_dict_get_encoding (GyDict *dict);
GyDict *gy_dict_new_object (const gchar *id_string);

void   gy_dict_map              (GyDict  *self,
                                 GError **err);
gchar* gy_dict_get_lexical_unit (GyDict *self,
                                 guint   index);

G_END_DECLS

#endif /* _GY_DICTIONARY_H_ */

