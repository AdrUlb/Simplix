#pragma once
#include <cstdint>
#include <limine.h>

#include "Silly/Macros.hpp"

extern volatile uint64_t limine_base_revision[];

#define LOADER_REQUEST(type, name, id) USED __attribute__((section(".limine_requests"))) volatile type LoaderRequests::name = { id, 0, nullptr };
#define LOADER_REQUEST_DECL(type, name) namespace LoaderRequests { extern volatile type name; }
#define LOADER_RESPONSE(name) LoaderRequests::name.response

LOADER_REQUEST_DECL(limine_memmap_request, Hddm);
