/*************************************************************************/
/*  openvr_interface.h                                                   */
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

#ifndef OPENVR_INTERFACE_H
#define OPENVR_INTERFACE_H

#include <stdio.h> // If you don't know what this is for stop reading now.
#include "globals.h"
#include "scene/resources/texture.h"
#include "scene/main/node.h"
#include "core/math/frustum.h"

#include <openvr.h>

/**
  openVR_interface implements a bridge to give access to the openVR API
**/
class openVR_interface {
private:
	int                     	refcount;

	vr::IVRSystem *				hmd;
	vr::IVRRenderModels *		renderModels;
	vr::TrackedDevicePose_t		trackedDevicePose[ vr::k_unMaxTrackedDeviceCount ];
	vr::VRControllerState_t		trackedDeviceState[ vr::k_unMaxTrackedDeviceCount ];

	vr::TrackedDeviceIndex_t	ControllerIndices[ vr::k_unMaxTrackedDeviceCount ];
	vr::TrackedDeviceIndex_t	BaseStationIndices[ vr::k_unMaxTrackedDeviceCount ];

	void attach_device(uint32_t p_device_index);
	void detach_device(uint32_t p_device_index);

protected:

public:
	static openVR_interface* get_singleton(bool p_in_editor);
	static void release();

	openVR_interface(bool p_in_editor);
	~openVR_interface();

	void process();

	// mapping controllers and base stations to openvr ids
	vr::TrackedDeviceIndex_t getControllerIndex(uint32_t p_index);
	vr::TrackedDeviceIndex_t getBaseStationIndex(uint32_t p_index);

	// state of controllers
	bool isButtonPressed(vr::TrackedDeviceIndex_t p_tracked_device_index, vr::EVRButtonId p_button);
	Point2 getAxis(vr::TrackedDeviceIndex_t p_tracked_device_index, vr::EVRButtonId p_axis);
	Transform getUpdatedTransform(vr::TrackedDeviceIndex_t p_tracked_device_index);
	const char * getTrackedDeviceName(vr::TrackedDeviceIndex_t p_tracked_device_index, int pMaxLen = 1024) const;

	// rendering
	Point2 getRecommendedRenderTargetSize();
	Transform getHeadToEyeTransform(Frustum::Eyes p_eye);
	Frustum getEyeFrustum(Frustum::Eyes p_eye);
	void commitEyeTexture(Frustum::Eyes p_eye, Ref<Texture> p_texture);
};

#endif