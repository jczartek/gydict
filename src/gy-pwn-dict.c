/* gy-pwn-dict.c
 *
 * Copyright (C) 2016 Jakub Czartek <kuba@linux.pl>
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

#include "gy-pwn-dict.h"

typedef struct
{
} GyPwnDictPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GyPwnDict, gy_pwn_dict, GY_TYPE_DICT)

enum {
  PROP_0,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];

static void
gy_pwn_dict_query (GyPwnDict      *self,
                   GyDictPwnQuery *query)
{
  GyPwnDictClass *klass;

  g_return_if_fail (GY_IS_PWN_DICT (self));

  klass = GY_PWN_DICT_GET_CLASS (self);

  g_return_if_fail (klass->query != NULL);

  klass->query (self, query);
}

static gboolean
gy_pwn_dict_check_checksum (GyPwnDict  *self,
                            GFile      *file,
                            GError    **err)
{
  GyPwnDictClass *klass;

  g_return_val_if_fail (GY_IS_PWN_DICT (self), FALSE);

  klass = GY_PWN_DICT_GET_CLASS (self);

  g_return_val_if_fail (klass->check_checksum != NULL, FALSE);

  return klass->check_checksum (self, file, err);
}

static void
gy_pwn_dict_finalize (GObject *object)
{
  GyPwnDict *self = (GyPwnDict *)object;
  GyPwnDictPrivate *priv = gy_pwn_dict_get_instance_private (self);

  G_OBJECT_CLASS (gy_pwn_dict_parent_class)->finalize (object);
}

static void
gy_pwn_dict_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  GyPwnDict *self = GY_PWN_DICT (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_pwn_dict_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  GyPwnDict *self = GY_PWN_DICT (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_pwn_dict_class_init (GyPwnDictClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gy_pwn_dict_finalize;
  object_class->get_property = gy_pwn_dict_get_property;
  object_class->set_property = gy_pwn_dict_set_property;

  klass->query = NULL;
  klass->check_checksum = NULL;
}

static void
gy_pwn_dict_init (GyPwnDict *self)
{
}
