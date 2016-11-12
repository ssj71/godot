/*************************************************************************/
/*  openvr_hmd.h                                                         */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2016 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef OPENVR_HMD_H
#define OPENVR_HMD_H

#include "scene/resources/texture.h"
#include "scene/3d/spatial.h"
#include "openvr_interface.h"

class OpenVRHMD : public Spatial {
	OBJ_TYPE(OpenVRHMD, Spatial);

public:
	OpenVRHMD();
	~OpenVRHMD();

	Point2 get_recommended_rendertarget_size();
	Transform get_lefteye_global_transform();
	Transform get_righteye_global_transform();
	Rect2 get_lefteye_frustum();
	Rect2 get_righteye_frustum();

	void set_lefteye_viewport_path(const NodePath& p_viewport);
	NodePath get_lefteye_viewport_path() const;
	void set_righteye_viewport_path(const NodePath& p_viewport);
	NodePath get_righteye_viewport_path() const;

	virtual String get_configuration_warning() const;

private:
	Ref<Texture> lefteye_texture;
	NodePath lefteye_viewport_path;

	Ref<Texture> righteye_texture;
	NodePath righteye_viewport_path;

	openVR_interface * ovr;

protected:
	static void _bind_methods();

	void process(float pDelta);
	void _notification(int p_what);
};

#endif