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
#include "gy-depl.h"
#include "gy-pwn-dict.h"
#include "gy-english-pwn.h"
#include "gy-german-pwn.h"
#include "gy-dict-history.h"
#include "history/gy-history.h"

typedef struct _GyDictPrivate
{
  gchar          *identifier;
  GtkTreeModel   *model;
  GyDictHistory  *history;
  guint           is_mapped: 1;
  guint           is_used:   1;
} GyDictPrivate;

enum
{
  PROP_0,
  PROP_IDENTIFIER,
  PROP_MODEL,
  PROP_IS_MAPPED,
  PROP_HISTORY,
  PROP_IS_USED,
  LAST_PROP
};

GParamSpec *gParamSpecs[LAST_PROP];

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GyDict, gy_dict, G_TYPE_OBJECT);

static void
gy_dict_dispose (GObject *object)
{
  GyDictPrivate *priv = gy_dict_get_instance_private (GY_DICT (object));

  if (priv->model)
    g_clear_object (&priv->model);

  G_OBJECT_CLASS (gy_dict_parent_class) -> dispose (object);
}
static void
gy_dict_finalize (GObject *object)
{
  GyDictPrivate *priv = gy_dict_get_instance_private (GY_DICT (object));

  if (priv->identifier)
    g_clear_pointer (&priv->identifier, g_free);

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
    case PROP_HISTORY:
      priv->history = g_value_dup_object (value);
      break;
    case PROP_IS_MAPPED:
      priv->is_mapped = g_value_get_boolean (value);
      break;
    case PROP_IS_USED:
      priv->is_used = g_value_get_boolean (value);
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
    case PROP_HISTORY:
      g_value_set_object (value, priv->history);
      break;
    case PROP_IS_MAPPED:
      g_value_set_boolean (value, priv->is_mapped);
      break;
    case PROP_IS_USED:
      g_value_set_boolean (value, priv->is_used);
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
  object_class->dispose = gy_dict_dispose;
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

  /**
   *
   * GyDict:history:
   *
   */
  gParamSpecs[PROP_HISTORY] =
    g_param_spec_object ("history",
                         "History",
                         "The history of a dictionary.",
                         GY_TYPE_DICT_HISTORY,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   *
   * GyDict:is-used:
   *
   * Is the dict being used at the moment?
   */
  gParamSpecs[PROP_IS_USED] =
    g_param_spec_boolean ("is-used",
                          "Is-used",
                          "Is the dict being used at the moment.",
                          FALSE,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, LAST_PROP, gParamSpecs);

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
gy_dict_is_mapped (GyDict *self)
{
  GyDictPrivate *priv;

  g_return_val_if_fail (GY_IS_DICT (self), FALSE);

  priv = gy_dict_get_instance_private (self);

  return (gboolean) priv->is_mapped;
}

gboolean
gy_dict_is_used (GyDict *self)
{
  GyDictPrivate *priv;

  g_return_val_if_fail (GY_IS_DICT (self), FALSE);

  priv = gy_dict_get_instance_private (self);

  return (gboolean) priv->is_used;
}

GtkTreeModel *
gy_dict_get_tree_model (GyDict *dict)
{
  GyDictPrivate *priv;
  g_return_val_if_fail (GY_IS_DICT (dict), NULL);

  priv = gy_dict_get_instance_private (dict);
  return priv->model;
}

GObject *
gy_dict_new (const gchar   *identifier)
{
  GType gtype;
  GObject *object = NULL;
  g_autofree gchar *type_name = NULL;
  g_autofree gchar *id = NULL;
  gsize offset = 0;

  g_return_val_if_fail (identifier != NULL, NULL);

  offset = strcspn (identifier, "-");
  type_name = g_strndup (identifier, offset);

  gtype = g_type_from_name (type_name);

  g_assert (gtype != 0);

  id = g_utf8_strdown (identifier, -1);

  object = g_object_new (gtype, "identifier", id, NULL);

  return object;
}

void
gy_dict_initialize (void)
{
  g_type_ensure (GY_TYPE_DICT);
  g_type_ensure (GY_TYPE_DEPL);
  g_type_ensure (GY_TYPE_PWN_DICT);
  g_type_ensure (GY_TYPE_ENGLISH_PWN);
  g_type_ensure (GY_TYPE_GERMAN_PWN);
}
