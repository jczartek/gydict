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

#include "gy-dict.h"
#include "../gy-dict-debug.h"


typedef struct _GyDictPrivate
{
  gchar          *identifier;
  GtkTreeModel   *model;
  guint           is_mapped: 1;
} GyDictPrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_CODE (GyDict, gy_dict, G_TYPE_OBJECT, G_ADD_PRIVATE (GyDict))

enum
{
  PROP_0,
  PROP_IDENTIFIER,
  PROP_MODEL,
  PROP_IS_MAPPED,
  LAST_PROP
};
GParamSpec *gParamSpecs[LAST_PROP];

static void
gy_dict_finalize (GObject *object)
{
  GyDictPrivate *priv = gy_dict_get_instance_private (GY_DICT (object));

  if (priv->identifier)
    g_clear_pointer (&priv->identifier, g_free);

  if (priv->model)
    g_clear_object (&priv->model);

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
    case PROP_IDENTIFIER:
      priv->identifier = g_value_dup_string (value);
      break;
    case PROP_MODEL:
      priv->model = g_value_dup_object (value);
      break;
    case PROP_IS_MAPPED:
      priv->is_mapped = g_value_get_boolean (value);
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
    case PROP_IDENTIFIER:
      g_value_set_static_string (value, priv->identifier);
      break;
    case PROP_MODEL:
      g_value_set_object (value, priv->model);
      break;
    case PROP_IS_MAPPED:
      g_value_set_boolean (value, priv->is_mapped);
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

  priv->model = NULL;
  priv->is_mapped = FALSE;
}

static void
gy_dict_class_init (GyDictClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gy_dict_finalize;
  object_class->set_property = gy_dict_set_property;
  object_class->get_property = gy_dict_get_property;

  klass->map = NULL;

  /**
   * GyDict:identifier:
   */
  gParamSpecs[PROP_IDENTIFIER] =
    g_param_spec_string ("identifier",
                         "Identifier",
                         "An identifier of a dictionary.",
                         NULL,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_CONSTRUCT_ONLY);

  /**
   * GyDict:model:
   *
   * The model for the tree view.
   */
  gParamSpecs[PROP_MODEL] =
     g_param_spec_object ("model",
                          "Model",
                          "The model for the tree view.",
                          GTK_TYPE_TREE_MODEL,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * GyDict:is-mapped:
   *
   * Is the object dict mapped.
   */
  gParamSpecs[PROP_IS_MAPPED] =
    g_param_spec_boolean ("is-mapped",
                          "Is-mapped",
                          "Is the dict mapped",
                          FALSE,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, LAST_PROP, gParamSpecs);

}

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

/**
 * gy_dict_parse:
 * @self: a gydict object
 * @raw_text: raw text to parse
 * @length: length of @raw_text, or -1 if nul terminated
 * @attr_list: (out): addres of return location for text with for a #PangoAttrList, or %NULL
 * @text: (out): addres of loaction for text with tags stripped, or %NULL
 * @err: addres of return location for errors, or %NULL
 *
 * Return value: %FALSE if @err is set, otherwise %TRUE
 */
gboolean
gy_dict_parse (GyDict         *self,
               const gchar    *raw_text,
               gint            length,
               PangoAttrList **attr_list,
               gchar         **text,
               GError        **err)
{
  GyDictClass *klass;

  g_return_val_if_fail (GY_IS_DICT (self), FALSE);
  g_return_val_if_fail (g_utf8_validate (raw_text, -1, NULL), FALSE);

  klass = GY_DICT_GET_CLASS (self);

  g_return_val_if_fail (klass->parse != NULL, FALSE);

  if (length == -1)
    length = g_utf8_strlen (raw_text, -1);

  return klass->parse (self, raw_text, length, attr_list, text, err);
}


gchar *
gy_dict_get_lexical_unit (GyDict  *self,
                          guint    idx,
                          GError **err)
{
  GyDictClass *klass;

  g_return_val_if_fail (GY_IS_DICT (self), NULL);

  klass = GY_DICT_GET_CLASS (self);

  g_return_val_if_fail (klass->get_lexical_unit != NULL, NULL);

  return klass->get_lexical_unit (self, idx, err);
}

gboolean
gy_dict_is_mapped (GyDict *self)
{
  GyDictPrivate *priv;

  g_return_val_if_fail (GY_IS_DICT (self), FALSE);

  priv = gy_dict_get_instance_private (self);

  return (gboolean) priv->is_mapped;
}

/**
 * gy_dict_get_tree_model:
 * @self: a #GyDict
 *
 * Returns the model #GtkTreeView that was created by the @self. Returns %NULL if the @self
 * is not mapped.
 *
 * Returns: (transfer none) (nullable): A #GtkTreeModel, or %NULL if the @self is not mapped.
 **/
GtkTreeModel *
gy_dict_get_tree_model (GyDict *self)
{
  GyDictPrivate *priv;
  g_return_val_if_fail (GY_IS_DICT (self), NULL);

  priv = gy_dict_get_instance_private (self);
  return priv->model;
}
