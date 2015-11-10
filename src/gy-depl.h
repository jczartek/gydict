/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * gy-depl.h
 * Copyright (C) 2014 kuba <kuba@linux.pl>
 *
 * gy-depl.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * gy-depl.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GY_DEPL_H_
#define _GY_DEPL_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_DEPL             (gy_depl_get_type ())
#define GY_DEPL(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GY_TYPE_DEPL, GyDepl))
#define GY_DEPL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GY_TYPE_DEPL, GyDeplClass))
#define GY_IS_DEPL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GY_TYPE_DEPL))
#define GY_IS_DEPL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GY_TYPE_DEPL))
#define GY_DEPL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GY_TYPE_DEPL, GyDeplClass))

typedef struct _GyDepl GyDepl;
typedef struct _GyDeplClass GyDeplClass;

struct _GyDepl
{
  GyDict parent_instance;
};

struct _GyDeplClass
{
  GyDictClass parent_class;
};

GType gy_depl_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* _GY_DEPL_H_ */
