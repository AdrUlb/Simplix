#include "Loader.hpp"

static USED __attribute__((section(".limine_requests"))) volatile uint64_t baseRevision[] = LIMINE_BASE_REVISION(6);
static USED __attribute__((section(".limine_requests_start"))) volatile uint64_t startMarker[] = LIMINE_REQUESTS_START_MARKER;
static USED __attribute__((section(".limine_requests_end"))) volatile uint64_t endMarker[] = LIMINE_REQUESTS_START_MARKER;

LOADER_REQUEST(limine_memmap_request, Hddm, LIMINE_MEMMAP_REQUEST_ID);
