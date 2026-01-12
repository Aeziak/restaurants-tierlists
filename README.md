# Restaurant Tier List

A geo-based collaborative tier list app for restaurants with PWA capabilities.

## Tech Stack

- **Frontend**: Next.js 16.1.1 with TypeScript, Tailwind CSS
- **Backend**: C++ Drogon 1.9.1 framework
- **Database**: PostgreSQL 16
- **Cache**: Redis 7
- **Export Service**: Node.js 20
- **Infrastructure**: Docker Compose

## Quick Start

### Prerequisites

- Docker & Docker Compose
- Node.js 20+ and pnpm (for local frontend development)
- GCC 13+ (for local backend development)

### Run with Docker

```bash
# Start all services
docker-compose up -d

# View logs
docker-compose logs -f

# Stop all services
docker-compose down
```

Services will be available at:
- **Frontend**: http://localhost:3000
- **Backend API**: http://localhost:8080
- **Export Service**: http://localhost:3001
- **PostgreSQL**: localhost:5432
- **Redis**: localhost:6379

### Local Development (Frontend only)

```bash
pnpm install
pnpm dev
```

Open [http://localhost:3000](http://localhost:3000) with your browser.

## Project Structure

```
restaurant-tier-list/
├── app/                    # Next.js app directory
├── backend/                # C++ Drogon backend
│   └── src/
│       ├── controllers/
│       ├── services/
│       ├── middleware/
│       ├── models/
│       └── utils/
├── export-service/         # Node.js image generation service
├── public/                 # Static assets
└── docker-compose.yml      # Container orchestration
```

## Architecture

See architecture documentation in `_bmad-output/planning-artifacts/` for detailed decisions and patterns.

## Development Status

Epic 1 - Infrastructure: In Progress
- ✅ Story 1.1: Next.js initialized
- ✅ Story 1.2: Dependencies installed
- ✅ Story 1.3: Backend structure created
- 🚧 Story 1.4: Docker Compose (current)

## Learn More

- [Next.js Documentation](https://nextjs.org/docs)
- [Drogon Documentation](https://github.com/drogonframework/drogon)

## License

Private project

