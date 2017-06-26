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

typedef struct _GyDictPrivate
{
  gchar          *identifier;
  GtkTreeModel   *model;
  GPtrArray      *h;
  guint           is_mapped: 1;
  guint           is_used:   1;
} GyDictPrivate;

enum
{
  ITEM_ADDED,
  LAST_SIGNAL
};
static guint signals[LAST_SIGNAL];

enum
{
  PROP_0,
  PROP_IDENTIFIER,
  PROP_MODEL,
  PROP_IS_MAPPED,
  PROP_IS_USED,
  LAST_PROP
};

GParamSpec *gParamSpecs[LAST_PROP];

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GyDict, gy_dict, G_TYPE_OBJECT);

static void
gy_dict_finalize (GObject *object)
{
  GyDictPrivate *priv = gy_dict_get_instance_private (GY_DICT (object));

  if (priv->identifier)
    g_clear_pointer (&priv->identifier, g_free);

  if (priv->model)
    g_clear_object (&priv->model);

  if (priv->h)
    g_clear_pointer (&priv->h, g_ptr_array_unref);

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
  priv->h = g_ptr_array_new_with_free_func ( (GDestroyNotify) g_variant_unref);
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

  /**
   * GyDict::item-added
   * @self: a GyDict
   * @item: a GVariant
   *
   */
  signals[ITEM_ADDED] =
    g_signal_new ("item-added",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                  0, NULL, NULL,
                  g_cclosure_marshal_VOID__VARIANT,
                  G_TYPE_NONE,
                  1, G_TYPE_VARIANT);

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
gy_dict_new (const gchar *identifier)
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
gy_dict_add_to_history (GyDict      *self,
                        const gchar *entry,
                        gint         n_row)
{
  GyDictPrivate *priv;

  g_return_if_fail (GY_IS_DICT (self));
  g_return_if_fail (entry != NULL && n_row >= 0);

  priv = gy_dict_get_instance_private (self);

  if (priv->h)
    {
      GVariant *variant = g_variant_new ("(si)", entry, n_row);
      g_ptr_array_add (priv->h, g_variant_ref_sink (variant));

      g_signal_emit (self, signals[ITEM_ADDED], 0, variant);
    }
}

void
gy_dict_foreach_history (GyDict   *self,
                         GFunc     func,
                         gpointer  data)
{
  GyDictPrivate *priv;

  g_return_if_fail (GY_IS_DICT (self));

  priv = gy_dict_get_instance_private (self);

  if (priv->h)
    {
      g_ptr_array_foreach (priv->h, func, data);
    }
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
