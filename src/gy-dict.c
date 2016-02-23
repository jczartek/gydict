/* gy-dict.c
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


#include "config.h"
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "gy-dict.h"
#include "gy-pwntabs.h"
#include "gy-pwn.h"
#include "gy-depl.h"
#include "gy-enum-types.h"
#include "gy-german-pwn.h"

typedef struct _GyDictPrivate GyDictPrivate;
typedef struct _GyDictionary GyDictionary;

struct _GyDictPrivate
{
  GtkTreeModel   *model;
  GyDictEncoding  encoding;
  gchar          *id_string;

  guint           is_map:1;
};

enum
{
  PROP_0,
  PROP_TREE_MODEL,
  PROP_ID_STRING,
  PROP_ENCODING,
  PROP_IS_MAP
};

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GyDict, gy_dict, G_TYPE_OBJECT);

static guint
set_dictionary_unimplemented (GyDict *dict)
{
  g_warning ("GyDictClass::set_dictionary not implemented for %s",
             g_type_name (G_TYPE_FROM_INSTANCE (dict)));

  return G_IO_ERROR_FAILED;

}

static guint
init_list_unimplemented (GyDict *dict)
{
  g_warning ("GyDictClass::init_list not implemented for %s",
             g_type_name (G_TYPE_FROM_INSTANCE (dict)));

  return G_IO_ERROR_FAILED;
}

static void
gy_dict_finalize (GObject *object)
{
  GyDictPrivate *priv;
  priv = gy_dict_get_instance_private (GY_DICT (object));

  if (priv->model)
    g_clear_object (&priv->model);
  if (priv->id_string)
    g_clear_pointer ((gpointer *) &priv->id_string,
                     g_free);
  G_OBJECT_CLASS (gy_dict_parent_class)->finalize (object);
}

static void
gy_dict_set_property (GObject      *object,
                      guint         prop_id,
                      const GValue *value,
                      GParamSpec   *pspec)
{
  GyDictPrivate *priv;
  g_return_if_fail (GY_IS_DICT (object));
  priv = gy_dict_get_instance_private (GY_DICT (object));

  switch (prop_id)
    {
      case PROP_TREE_MODEL:
      gy_dict_set_tree_model (GY_DICT (object),
                              GTK_TREE_MODEL (g_value_get_object (value)));
      break;
      case PROP_ENCODING:
      priv->encoding = g_value_get_enum (value);
      break;
      case PROP_ID_STRING:
      priv->id_string = g_value_dup_string (value);
      break;
      case PROP_IS_MAP:
      priv->is_map = g_value_get_boolean (value);
      break;
      default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gy_dict_get_property (GObject    *object, 
                      guint       prop_id,
                      GValue     *value,
                      GParamSpec *pspec)
{
  GyDictPrivate *priv;
  g_return_if_fail (GY_IS_DICT (object));
  priv = gy_dict_get_instance_private (GY_DICT (object));

  switch (prop_id)
    {
      case PROP_TREE_MODEL:
      g_value_take_object (value, priv->model);
      break;
      case PROP_ENCODING:
      g_value_set_enum (value, priv->encoding);
      break;
      case PROP_ID_STRING:
      g_value_set_string (value, priv->id_string);
      break;
      case PROP_IS_MAP:
      g_value_set_boolean (value, priv->is_map);
      break;
      default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
      }
}

static void
gy_dict_init (GyDict *dict)
{
  GyDictPrivate *priv = gy_dict_get_instance_private (dict);

  priv->id_string = NULL;
  priv->encoding = GY_ENCODING_NONE;
  priv->model = NULL;
  priv->is_map = FALSE;
}

static void
gy_dict_class_init (GyDictClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gy_dict_finalize;
  object_class->set_property = gy_dict_set_property;
  object_class->get_property = gy_dict_get_property;

  klass->set_dictionary = set_dictionary_unimplemented;
  klass->init_list = init_list_unimplemented;
  klass->map = NULL;

  g_object_class_install_property (object_class,
                                   PROP_TREE_MODEL,
                                   g_param_spec_object ("tree-model",
                                                        "tree-model",
                                                        "tree model for dictionary",
                                                        GTK_TYPE_TREE_MODEL,
                                                        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (object_class,
                                   PROP_ID_STRING,
                                   g_param_spec_string ("id-dict-string",
                                                        "id-dict-string",
                                                        "id string dictionary", NULL,
                                                        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_CONSTRUCT_ONLY));
  g_object_class_install_property (object_class,
                                   PROP_ENCODING,
                                   g_param_spec_enum ("encoding-dict",
                                                      "encoding-dict",
                                                      "encoding dict",
                                                      GY_TYPE_ENUM_ENCODING,
                                                      GY_ENCODING_NONE,
                                                      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (object_class,
                                   PROP_IS_MAP,
                                   g_param_spec_boolean ("is-map",
                                                         "Is map",
                                                         "Whether the dict is map",
                                                         FALSE,
                                                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

}

/***************************FUBLIC METHOD***************************/
void
gy_dict_map (GyDict  *self,
             GError **err)
{
  GyDictClass *klass;
  g_return_if_fail (GY_IS_DICT (self));

  klass = GY_DICT_GET_CLASS (self);

  g_return_if_fail (klass->map != NULL);

  klass->map (self, err);
}

gboolean
gy_dict_is_map (GyDict *self)
{
  GyDictPrivate *priv;

  g_return_val_if_fail (GY_IS_DICT (self), FALSE);

  priv = gy_dict_get_instance_private (self);

  return (gboolean) priv->is_map;
}

guint
gy_dict_set_dictionary (GyDict *dict)
{
  guint error;

  error = GY_DICT_GET_CLASS (dict)->set_dictionary (dict);

  return error;
}

guint
gy_dict_init_list (GyDict *dict)
{
  guint error;

  error = GY_DICT_GET_CLASS (dict)->init_list (dict);

  return error;
}

void
gy_dict_set_tree_model (GyDict       *dict,
                        GtkTreeModel *model)
{
  GyDictPrivate *priv;
  g_return_if_fail (GY_IS_DICT (dict));
  g_return_if_fail (GTK_IS_TREE_MODEL (model));

  priv = gy_dict_get_instance_private (dict);
  g_object_freeze_notify (G_OBJECT (dict));

  if (!priv->model)
    {
      priv->model = model;
      g_object_notify (G_OBJECT (dict), "tree-model");
    }

  g_object_thaw_notify (G_OBJECT (dict));
}

GtkTreeModel *
gy_dict_get_tree_model (GyDict *dict)
{
  GyDictPrivate *priv;
  g_return_val_if_fail (GY_IS_DICT (dict), NULL);

  priv = gy_dict_get_instance_private (dict);
  return priv->model;
}

const gchar *
gy_dict_get_id_string (GyDict *dict)
{
  GyDictPrivate *priv;
  g_return_val_if_fail (GY_IS_DICT (dict), NULL);

  priv = gy_dict_get_instance_private (dict);
  return (const gchar *) priv->id_string;
}

gint
gy_dict_get_encoding (GyDict *dict)
{
  GyDictPrivate *priv;
  g_return_val_if_fail (GY_IS_DICT (dict), GY_ENCODING_NONE);

  priv = gy_dict_get_instance_private (dict);
  return priv->encoding;
}

GyDict *
gy_dict_new_object (const gchar *id_string)
{
  if ((g_strcmp0 (id_string, "dict-pwn-angpol") == 0) ||
      (g_strcmp0 (id_string, "dict-pwn-polang") == 0))
    {
      return GY_DICT (g_object_new (GY_TYPE_PWN, "id-dict-string", id_string, NULL));
    }
  else if ((g_strcmp0 (id_string, "dict-pwn-polniem") == 0) ||
           (g_strcmp0 (id_string, "dict-pwn-niempol") == 0))
    {
      return GY_DICT (g_object_new (GY_TYPE_GERMAN_PWN, "id-dict-string", id_string, NULL));
    }
  else
    {
      return GY_DICT (g_object_new (GY_TYPE_DEPL, "id-dict-string", id_string, NULL));
    }
}
