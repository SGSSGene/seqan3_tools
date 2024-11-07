# SPDX-FileCopyrightText: 2006-2023, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2023, Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: CC0-1.0

cmake_minimum_required (VERSION 3.8)

## CUSTOMISE

# Define the application name and version.
project (seqan3_tools VERSION 1.0.0)

# Specify the directories where to store the built archives, libraries and executables
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include(cmake/CPM.cmake)
CPMAddPackage("gh:seqan/seqan3#4d03890530089b040221876c9e368fc250c4583f")
set(BUILD_DIVSUFSORT64 ON)
CPMAddPackage("gh:y-256/libdivsufsort#5f60d6f026c30fb4ac296f696b3c8b0eb71bd428")

# Add the application.
add_subdirectory (src)
