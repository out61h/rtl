/*
 * Copyright (C) 2016-2023 Konstantin Polevik
 * All rights reserved
 *
 * This file is part of the RTL library. Redistribution and use in source and
 * binary forms, with or without modification, are permitted exclusively
 * under the terms of the MIT license. You should have received a copy of the
 * license with this file. If not, please visit:
 * https://github.com/out61h/rtl/blob/main/LICENSE.
 */
#pragma once

#define RTL_IMPLEMENTATION

#include "impl/application.hpp"
#include "impl/chrono.hpp"
#include "impl/debug.hpp"
#include "impl/filesystem.hpp"
#include "impl/memory.hpp"
#include "impl/printf.hpp"
#include "impl/startup.hpp"
#include "impl/string.hpp"

#include "impl/app/audio.hpp"
#include "impl/app/environment.hpp"
#include "impl/app/opengl.hpp"
#include "impl/app/osd.hpp"
#include "impl/app/proc.hpp"
#include "impl/app/resize.hpp"
#include "impl/app/resources.hpp"
#include "impl/app/screen_buffer.hpp"

#include "impl/opencl/buffer.hpp"
#include "impl/opencl/context.hpp"
#include "impl/opencl/device.hpp"
#include "impl/opencl/kernel.hpp"
#include "impl/opencl/platform.hpp"
#include "impl/opencl/program.hpp"

#undef RTL_IMPLEMENTATION
