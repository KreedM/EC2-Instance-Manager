# This file is part of EC2-Instance-Manager
# Copyright (C) 2025  Derek Meng

# EC2-Instance-Manager is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# EC2-Instance-Manager is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public License
# along with EC2-Instance-Manager.  If not, see <https://www.gnu.org/licenses/>.

set(
    sources
    "EC2Utils.cpp"
    "EC2InstanceManager.cpp"
)

add_library(EC2-Utils STATIC ${sources})

target_include_directories(EC2-Utils PUBLIC "${PROJECT_SOURCE_DIR}/include")
