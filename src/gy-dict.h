/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * gy-dictionary.h
 * Copyright (C) 2014 kuba <kuba@fedora>
 *
 * gy-dictionary.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * gy-dictionary.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GY_DICTIONARY_H_
#define _GY_DICTIONARY_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_DICT             (gy_dict_get_type ())
#define GY_DICT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GY_TYPE_DICT, GyDict))
#define GY_DICT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GY_TYPE_DICT, GyDictClass))
#define GY_IS_DICT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GY_TYPE_DICT))
#define GY_IS_DICT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GY_TYPE_DICT))
#define GY_DICT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GY_TYPE_DICT, GyDictClass))

/*typedef enum
{
    GY_ENCODING_NONE = -1,
    GY_ENCODING_CP1250,
    GY_ENCODING_ISO88592,
    GY_ENCODING_UTF8
} GyDictEncoding;*/

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

typedef struct _GyDict GyDict;
typedef struct _GyDictClass GyDictClass;

struct _GyDict
{
  GObject parent_instance;
};

struct _GyDictClass
{
  GObjectClass parent_class;

  /* Virtual Method */
  guint     (*set_dictionary)   (GyDict *dict);
  guint     (*init_list)        (GyDict *dict);
  gpointer  (*read_definition)  (GyDict *dict,
                                 guint   index);

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

G_END_DECLS

#endif /* _GY_DICTIONARY_H_ */

