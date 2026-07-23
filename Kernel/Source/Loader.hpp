#pragma once
#include <cstdint>
#include <limine.h>

#include "Silly/Macros.hpp"

#define LOADER_REQUEST(type, name, id) USED __attribute__((section(".limine_requests"))) volatile type Requests::name = { id, 0, nullptr };
#define LOADER_REQUEST_DECL(type, name) namespace Requests { extern volatile type name; }
#define LOADER_RESPONSE(name) Kernel::Loader::Requests::name.response

namespace Kernel::Loader
{
	extern volatile uint64_t limine_base_revision[];
	LOADER_REQUEST_DECL(limine_memmap_request, Hddm);
}
