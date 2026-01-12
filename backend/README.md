# Restaurant Tier List - Backend

C++ backend using Drogon framework 1.9.1

## Project Structure

```
backend/
├── CMakeLists.txt          # CMake configuration
├── src/
│   ├── main.cpp            # Application entry point
│   ├── controllers/        # HTTP controllers
│   ├── services/           # Business logic layer
│   ├── middleware/         # HTTP middleware
│   ├── models/             # Data models
│   └── utils/              # Utility functions
└── build/                  # Build artifacts (git-ignored)
```

## Dependencies

- **Drogon 1.9.1** - C++ web framework
- **jwt-cpp** - JWT token validation
- **redis-plus-plus** - Redis client
- **PostgreSQL (libpq)** - Database client

## Database Migrations

See `migrations/README.md` for details on applying database migrations.

Initial migration `000_init_database.sql` sets up:
- UTF-8 encoding
- UTC timezone
- Migration tracking table

## Build Instructions

### Prerequisites

- GCC 13+ or compatible C++20 compiler
- CMake 3.15+
- vcpkg or system-installed libraries

**Note:** For local development, dependencies can be complex to install. See Docker setup (Story 1.4) for containerized build environment with all dependencies pre-configured.

### Build (Native)

```bash
cd backend
cmake -B build -S .
cmake --build build/
```

### Build (Docker - Recommended)

See docker-compose.yml in project root (configured in Story 1.4).

### Run

```bash
./build/restaurant_tier_list_backend
```

Server will start on http://localhost:8080

## Architecture

Follows layered architecture pattern:
- **Controllers**: Handle HTTP requests/responses
- **Services**: Business logic and data orchestration
- **Middleware**: Authentication, logging, error handling
- **Models**: Data structures and validation
- **Utils**: Shared utilities
