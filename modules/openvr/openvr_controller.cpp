/*************************************************************************/
/*  openvr_controller.cpp                                                */
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

#include "openvr_controller.h"

#include <stdio.h>
#include <string.h>

#include "scene/resources/material.h"
#include "scene/resources/surface_tool.h"
 
void OpenVRController::_bind_methods() {
	ObjectTypeDB::bind_method( _MD("get_controller_index"),&OpenVRController::get_controller_index );
	ObjectTypeDB::bind_method( _MD("set_controller_index","controller_index"),&OpenVRController::set_controller_index );
	ObjectTypeDB::bind_method( _MD("get_device_name"),&OpenVRController::get_device_name );
	ObjectTypeDB::bind_method( _MD("get_button_system"),&OpenVRController::get_button_system );
	ObjectTypeDB::bind_method( _MD("get_button_appmenu"),&OpenVRController::get_button_appmenu );
	ObjectTypeDB::bind_method( _MD("get_button_grip"),&OpenVRController::get_button_grip );
	ObjectTypeDB::bind_method( _MD("get_dpad_left"),&OpenVRController::get_dpad_left );
	ObjectTypeDB::bind_method( _MD("get_dpad_up"),&OpenVRController::get_dpad_up );
	ObjectTypeDB::bind_method( _MD("get_dpad_right"),&OpenVRController::get_dpad_right );
	ObjectTypeDB::bind_method( _MD("get_dpad_down"),&OpenVRController::get_dpad_down );
	ObjectTypeDB::bind_method( _MD("get_button_a"),&OpenVRController::get_button_a );
	ObjectTypeDB::bind_method( _MD("get_touchpad"),&OpenVRController::get_touchpad );
	ObjectTypeDB::bind_method( _MD("get_trigger"),&OpenVRController::get_trigger );
	ObjectTypeDB::bind_method( _MD("get_axis0"),&OpenVRController::get_axis0 );
	ObjectTypeDB::bind_method( _MD("get_axis1"),&OpenVRController::get_axis1 );
	ObjectTypeDB::bind_method( _MD("get_axis2"),&OpenVRController::get_axis2 );
	ObjectTypeDB::bind_method( _MD("get_axis3"),&OpenVRController::get_axis3 );
	ObjectTypeDB::bind_method( _MD("get_axis4"),&OpenVRController::get_axis4 );

	// temporary workaround until we implement read only properties in Godot 3
	ObjectTypeDB::bind_method( _MD("read_only_float", "dummy"),&OpenVRController::read_only_float );
	ObjectTypeDB::bind_method( _MD("read_only_int", "dummy"),&OpenVRController::read_only_int );
	ObjectTypeDB::bind_method( _MD("read_only_string", "dummy"),&OpenVRController::read_only_string );
	ObjectTypeDB::bind_method( _MD("read_only_point2", "dummy"),&OpenVRController::read_only_point2 );

	ADD_PROPERTYNZ( PropertyInfo( Variant::INT, "controller_index"), _SCS("set_controller_index"),_SCS("get_controller_index"));
	ADD_PROPERTY( PropertyInfo( Variant::STRING, "device_name"), _SCS("read_only_string"), _SCS("get_device_name"));
	ADD_PROPERTY( PropertyInfo( Variant::INT, "button_system"), _SCS("read_only_int"), _SCS("get_button_system"));
	ADD_PROPERTY( PropertyInfo( Variant::INT, "button_appmenu"), _SCS("read_only_int"), _SCS("get_button_appmenu"));
	ADD_PROPERTY( PropertyInfo( Variant::INT, "button_grip"), _SCS("read_only_int"), _SCS("get_button_grip"));
	ADD_PROPERTY( PropertyInfo( Variant::INT, "dpad_left"), _SCS("read_only_int"), _SCS("get_dpad_left"));
	ADD_PROPERTY( PropertyInfo( Variant::INT, "dpad_up"), _SCS("read_only_int"), _SCS("get_dpad_up"));
	ADD_PROPERTY( PropertyInfo( Variant::INT, "dpad_right"), _SCS("read_only_int"), _SCS("get_dpad_right"));
	ADD_PROPERTY( PropertyInfo( Variant::INT, "dpad_down"), _SCS("read_only_int"), _SCS("get_dpad_down"));
	ADD_PROPERTY( PropertyInfo( Variant::INT, "button_a"), _SCS("read_only_int"), _SCS("get_button_a"));

	ADD_PROPERTY( PropertyInfo( Variant::VECTOR2, "touchpad"), _SCS("read_only_point2"), _SCS("get_touchpad"));
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "trigger"), _SCS("read_only_float"), _SCS("get_trigger"));
	ADD_PROPERTY( PropertyInfo( Variant::VECTOR2, "axis0"), _SCS("read_only_point2"), _SCS("get_axis0"));
	ADD_PROPERTY( PropertyInfo( Variant::VECTOR2, "axis1"), _SCS("read_only_point2"), _SCS("get_axis1"));
	ADD_PROPERTY( PropertyInfo( Variant::VECTOR2, "axis2"), _SCS("read_only_point2"), _SCS("get_axis2"));
	ADD_PROPERTY( PropertyInfo( Variant::VECTOR2, "axis3"), _SCS("read_only_point2"), _SCS("get_axis3"));
	ADD_PROPERTY( PropertyInfo( Variant::VECTOR2, "axis4"), _SCS("read_only_point2"), _SCS("get_axis4"));

	ADD_SIGNAL( MethodInfo("activated",PropertyInfo(Variant::OBJECT,"controller")));
	ADD_SIGNAL( MethodInfo("deactivated",PropertyInfo(Variant::OBJECT,"controller")));

	ADD_SIGNAL( MethodInfo("button_pressed",PropertyInfo(Variant::OBJECT,"controller"),PropertyInfo(Variant::INT,"button")));
	ADD_SIGNAL( MethodInfo("button_released",PropertyInfo(Variant::OBJECT,"controller"),PropertyInfo(Variant::INT,"button")));
};

uint32_t OpenVRController::get_controller_index() const {
	return controller_index + 1; // we store from 0 but show from index 1
};

void OpenVRController::set_controller_index(uint32_t p_index) {
	if ((p_index > 0) && (p_index <= vr::k_unMaxTrackedDeviceCount)) {
		controller_index = p_index - 1;
	};
};

String OpenVRController::get_device_name() const {
	String ret;

	if (ovr != NULL) {
		ret = device_name;
	} else {
		ret = "Not initialized";
	};

	return ret;
};

int OpenVRController::get_button_system() const {
	if (ovr != NULL) {
		if (ovr->isButtonPressed(ovr->getControllerIndex(controller_index), vr::k_EButton_System)) {
			return 1;
		};
	};

	return 0;
};

int OpenVRController::get_button_appmenu() const {
	if (ovr != NULL) {
		if (ovr->isButtonPressed(ovr->getControllerIndex(controller_index), vr::k_EButton_ApplicationMenu)) {
			return 1;
		};
	};

	return 0;
};

int OpenVRController::get_button_grip() const {
	if (ovr != NULL) {
		if (ovr->isButtonPressed(ovr->getControllerIndex(controller_index), vr::k_EButton_Grip)) {
			return 1;
		};
	};

	return 0;
};

int OpenVRController::get_dpad_left() const {
	if (ovr != NULL) {
		if (ovr->isButtonPressed(ovr->getControllerIndex(controller_index), vr::k_EButton_DPad_Left)) {
			return 1;
		};
	};

	return 0;
};

int OpenVRController::get_dpad_up() const {
	if (ovr != NULL) {
		if (ovr->isButtonPressed(ovr->getControllerIndex(controller_index), vr::k_EButton_DPad_Up)) {
			return 1;
		};
	};

	return 0;
};

int OpenVRController::get_dpad_right() const {
	if (ovr != NULL) {
		if (ovr->isButtonPressed(ovr->getControllerIndex(controller_index), vr::k_EButton_DPad_Right)) {
			return 1;
		};
	};

	return 0;
};

int OpenVRController::get_dpad_down() const {
	if (ovr != NULL) {
		if (ovr->isButtonPressed(ovr->getControllerIndex(controller_index), vr::k_EButton_DPad_Down)) {
			return 1;
		};
	};

	return 0;
};

int OpenVRController::get_button_a() const {
	if (ovr != NULL) {
		if (ovr->isButtonPressed(ovr->getControllerIndex(controller_index), vr::k_EButton_A)) {
			return 1;
		};
	};

	return 0;
};

Point2 OpenVRController::get_touchpad() const {
	if (ovr != NULL) {
		return ovr->getAxis(ovr->getControllerIndex(controller_index), vr::k_EButton_SteamVR_Touchpad);
	};

	return Point2(0.0, 0.0);
};

float OpenVRController::get_trigger() const {
	if (ovr != NULL) {
		return ovr->getAxis(ovr->getControllerIndex(controller_index), vr::k_EButton_SteamVR_Trigger).x;
	};

	return 0.0;
};

Point2 OpenVRController::get_axis0() const {
	if (ovr != NULL) {
		return ovr->getAxis(ovr->getControllerIndex(controller_index), vr::k_EButton_Axis0);
	};

	return Point2(0.0, 0.0);
};

Point2 OpenVRController::get_axis1() const {
	if (ovr != NULL) {
		return ovr->getAxis(ovr->getControllerIndex(controller_index), vr::k_EButton_Axis1);
	};

	return Point2(0.0, 0.0);
};

Point2 OpenVRController::get_axis2() const {
	if (ovr != NULL) {
		return ovr->getAxis(ovr->getControllerIndex(controller_index), vr::k_EButton_Axis2);
	};

	return Point2(0.0, 0.0);
};

Point2 OpenVRController::get_axis3() const {
	if (ovr != NULL) {
		return ovr->getAxis(ovr->getControllerIndex(controller_index), vr::k_EButton_Axis3);
	};

	return Point2(0.0, 0.0);
};

Point2 OpenVRController::get_axis4() const {
	if (ovr != NULL) {
		return ovr->getAxis(ovr->getControllerIndex(controller_index), vr::k_EButton_Axis4);
	};

	return Point2(0.0, 0.0);
};

// temporary workaround until we have read only properties!
void OpenVRController::read_only_int(uint32_t p_int) {
	// do nothing, this is a read only property!
};

void OpenVRController::read_only_float(float p_float) {
	// do nothing, this is a read only property!
};

void OpenVRController::read_only_string(const String & p_string) {
	// do nothing, this is a read only property!
};

void OpenVRController::read_only_point2(Point2 p_point2) {
	// do nothing, this is a read only property!
};

void OpenVRController::process(float pDelta) {
	// @TODO: we should find a way to update all information about our trackers so we don't trickle this data in during processing..
	if (ovr != NULL) {
		vr::TrackedDeviceIndex_t tracked_device_index = ovr->getControllerIndex(controller_index);

		// update our tracked device data...
		set_transform(ovr->getUpdatedTransform(tracked_device_index));

		// check if we're initialized...
		bool currently_initialized = true;
		if (device_name[0] == '\0') {
			currently_initialized = false;
		} else if (strcmp(device_name, "Not initialized") == 0) {
			currently_initialized = false;
		};

		// Check of our device has changed...
		char new_device_name[256];
		strcpy(new_device_name, ovr->getTrackedDeviceName(tracked_device_index, 255));
		if (strcmp(device_name, new_device_name) != 0) {
			strcpy(device_name, new_device_name);
			update_gizmo();

			if (!currently_initialized) {
				emit_signal("activated", this);
			} else if (strcmp(device_name, "Not initialized") == 0) {
				emit_signal("deactivated", this);
			};
		};

		// update button states, this is a little double with what OpenVR does interally but...
		for (int i = 0; i < 16; i++) {
			bool is_pressed = ovr->isButtonPressed(tracked_device_index, (vr::EVRButtonId) i);
			if (button_pressed[i] != is_pressed) {
				button_pressed[i] = is_pressed;
				if (is_pressed) {
					emit_signal("button_pressed", this, i);
				} else {
					emit_signal("button_released", this, i);
				};
			}
		};

		// Done...
		return;
	};
};

void OpenVRController::_notification(int p_what) {
	switch (p_what ) {
		case NOTIFICATION_ENTER_TREE: {
			if (ovr == NULL) {
				ovr = openVR_interface::get_singleton(get_tree()->is_editor_hint());
			}

			set_process(true);
		}; break;
		case NOTIFICATION_EXIT_TREE: {
			set_process(false);

			if (ovr != NULL) {
				openVR_interface::release();
				ovr = NULL;
			};
		}; break;
		case NOTIFICATION_PROCESS: {
			process(get_process_delta_time());
		}; break;
		default : {
			// don't do anything here
		}; break;
	};
};

bool OpenVRController::_can_gizmo_scale() const {
	return false;
};

RES OpenVRController::_get_gizmo_geometry() const {
	Ref<SurfaceTool> surface_tool( memnew( SurfaceTool ));

	Ref<FixedMaterial> mat( memnew( FixedMaterial ));

	mat->set_parameter( FixedMaterial::PARAM_DIFFUSE,Color(1.0,0.5,0.5,1.0) );
	mat->set_line_width(4);
	mat->set_flag(Material::FLAG_DOUBLE_SIDED,true);
	mat->set_flag(Material::FLAG_UNSHADED,true);
	//mat->set_hint(Material::HINT_NO_DEPTH_DRAW,true);

	surface_tool->begin(Mesh::PRIMITIVE_LINES);
	surface_tool->set_material(mat);

	/* if we can find a rendermodel for our controller, we build it, else we add something basic */
	if ((device_name[0] != '\0') && (strcmp(device_name, "Not initialized") != 0)) {
		// load model
	};

//	if (model == loaded) {
// copy data into surface tool
//	} else {
		surface_tool->add_vertex(Vector3(-0.1, 0.0, 0.0));
		surface_tool->add_vertex(Vector3( 0.0, 0.1, 0.0));

		surface_tool->add_vertex(Vector3( 0.0, 0.1, 0.0));
		surface_tool->add_vertex(Vector3( 0.1, 0.0, 0.0));

		surface_tool->add_vertex(Vector3( 0.1, 0.0, 0.0));
		surface_tool->add_vertex(Vector3( 0.0,-0.1, 0.0));

		surface_tool->add_vertex(Vector3( 0.0,-0.1, 0.0));
		surface_tool->add_vertex(Vector3(-0.1, 0.0, 0.0));

//	};


	return surface_tool->commit();
};

OpenVRController::OpenVRController() {
	ovr = NULL;
	controller_index = 0;
	device_name[0] = '\0';
	memset(button_pressed, 0, sizeof(button_pressed));
};

OpenVRController::~OpenVRController() {
	if (ovr != NULL) {
		openVR_interface::release();
		ovr = NULL;
	};
};
