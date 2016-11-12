/*************************************************************************/
/*  openvr_controller.h                                                  */
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

#ifndef OPENVR_CONTROLLER_H
#define OPENVR_CONTROLLER_H

#include "scene/3d/spatial.h"
#include "openvr_interface.h"

class OpenVRController : public Spatial {
	OBJ_TYPE(OpenVRController, Spatial);

public:
	OpenVRController();
	~OpenVRController();

	uint32_t get_controller_index() const;
	void set_controller_index(uint32_t p_index);
	String get_device_name() const;
	int get_button_system() const;
	int get_button_appmenu() const;
	int get_button_grip() const;
	int get_dpad_left() const;
	int get_dpad_up() const;
	int get_dpad_right() const;
	int get_dpad_down() const;
	int get_button_a() const;

	Point2 get_touchpad() const; // axis 0
	float get_trigger() const; // axis 1, only use x
	Point2 get_axis0() const;
	Point2 get_axis1() const;
	Point2 get_axis2() const;
	Point2 get_axis3() const;
	Point2 get_axis4() const;

	void read_only_string(const String & p_string);
	void read_only_float(float p_float);
	void read_only_int(uint32_t p_int);
	void read_only_point2(Point2 p_point2);

private:
	char				device_name[256];
	openVR_interface *	ovr;
	uint32_t			controller_index;
	bool				button_pressed[16];

	virtual bool _can_gizmo_scale() const;
	virtual RES _get_gizmo_geometry() const;

protected:
	static void _bind_methods();

	void process(float pDelta);
	void _notification(int p_what);
};

#endif