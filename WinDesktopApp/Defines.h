#pragma once

#define RELEASE_AND_NULL(p) { if (p) { p->Release(); p = nullptr; } }
#define SHUTDOWN_DELETE(p) { if (p) { p->Shutdown(); delete p; p = nullptr; } }
#define DELETE(p) { delete p; p = nullptr; }
#define DELETE_ARR(p) { delete[] p; p = nullptr; }